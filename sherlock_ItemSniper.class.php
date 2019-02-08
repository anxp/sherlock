<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2018-12-20
 * Time: 08:09
 */

require_once 'phpQuery/phpQuery.php';

abstract class ItemSniper {
  //Just the most popular user agent, it can be overriden by setUserAgent() method:
  protected $userAgent = 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.110 Safari/537.36';

  //This value will be set to CURLOPT_CONNECTTIMEOUT. Can be overriden by setConnectionTimeout() method:
  //TODO: Read about connection timeout in cURL, and set something more correct (2 sec?)
  protected $connectionTimeout = 10;

  //URL to fetch. We will set it up in constructor:
  protected $URL = '';

  //How deep (in pages) our script will dig. 0 for UNLIMITED.
  protected $pageLimit = 0;

  //How to find advertisement block in webpage - search string for phpQuery ('SP' stands for 'Search Pattern'):
  protected $advertBlockSP = '';

  //How to find title of given advertisement block. This 'path' is not absolute, but relative to $advertBlockSP ('SP' stands for 'Search Pattern'):
  protected $titleSP = '';

  //How to find title link (link to item page with full description, 'SP' stands for 'Search Pattern'):
  protected $titleLinkSP = '';

  //How to find price of given advertisement block. This 'path' is not absolute, but relative to $advertBlockSP ('SP' stands for 'Search Pattern'):
  protected $priceSP = '';

  //How to find link to attached thumbnail image. This 'path' is not absolute, but relative to $advertBlockSP ('SP' stands for 'Search Pattern'):
  protected $imageAddressSP = '';

  //How to find link to next page in webpage with advertisements - search string for phpQuery ('SP' stands for 'Search Pattern'):
  protected $nextPageLinkSP = '';

  //$URL - URL to fetch;
  //We do constructor protected, because we want create only objects of child classes.
  protected function __construct($URL, $pageLimit, $advertBlockSP, $titleSP, $titleLinkSP, $priceSP, $imageAddressSP, $nextPageLinkSP) {
    $this->URL = $URL;
    $this->pageLimit = $pageLimit;
    $this->advertBlockSP = $advertBlockSP;
    $this->titleSP = $titleSP;
    $this->titleLinkSP = $titleLinkSP;
    $this->priceSP = $priceSP;
    $this->imageAddressSP = $imageAddressSP;
    $this->nextPageLinkSP = $nextPageLinkSP;
  }

  /**
   * @param string $userAgent
   */
  public function setUserAgent(string $userAgent): void {
    $this->userAgent = $userAgent;
  }

  /**
   * @param int $connectionTimeout
   */
  public function setConnectionTimeout(int $connectionTimeout): void {
    $this->connectionTimeout = $connectionTimeout;
  }

  /**
   * @return string
   */
  public function getUserAgent(): string {
    return $this->userAgent;
  }

  /**
   * @return int
   */
  public function getConnectionTimeout(): int {
    return $this->connectionTimeout;
  }

  /**
   * @return string
   * @param string $url
   */
  protected function getWebpageHtml($url) {
    $ch = curl_init();

    curl_setopt_array($ch, [
      CURLOPT_USERAGENT => $this->getUserAgent(),
      CURLOPT_CONNECTTIMEOUT => $this->getConnectionTimeout(),
      CURLOPT_RETURNTRANSFER => TRUE,
      CURLOPT_HEADER => FALSE,
      CURLOPT_FOLLOWLOCATION => TRUE, //Follow redirects
      CURLOPT_URL => $url,
    ]);

    //Get URL content in string:
    $htmlString = curl_exec($ch);

    //Close handle to release resources
    curl_close($ch);

    return $htmlString;
  }

  /**
   * @return array
   */
  abstract public function grabItems() : array; //Method for parsing. Need to be declared in child class, because realization depends on target market website
}

/*
 * Naming convention for child classes:
 * They must have prefix with marketId and underscore symbol, like: olx_, bsp_, skl_, with word 'ItemSniper' after prefix: ex.: olx_ItemSniper.
 * This makes possible to create new instances automatically, knowing only marketId.
 * */
class olx_ItemSniper extends ItemSniper {

  public function __construct($URL, $pageLimit, $advertBlockSP = 'div.offer-wrapper', $titleSP = 'h3', $titleLinkSP = 'h3 > a', $priceSP = 'p.price', $imageAddressSP = 'img.fleft', $nextPageLinkSP = 'div.pager span.next a') {
    parent::__construct($URL, $pageLimit, $advertBlockSP, $titleSP, $titleLinkSP, $priceSP, $imageAddressSP, $nextPageLinkSP);
  }

  public function grabItems(): array {
    $pgCount = 0;
    $collectedItems = []; //Here we will store all found items. This is indexed array with associative sub-arrays.
    $cii = 0; //cii == Collected Items Iterator.
    $url = $this->URL;
    do {
      $fullHTML = $this->getWebpageHtml($url);
      $doc = phpQuery::newDocument($fullHTML);
      $offerBlocks = $doc->find($this->advertBlockSP);
      foreach ($offerBlocks as $block) {
        $pq_block = pq($block);

        //Get title of the ad-block:
        $itemTitle = $pq_block->find($this->titleSP)->text();

        //Get address of attached thumbnail:
        $pq_block->find($this->imageAddressSP)->removeAttr('data-src');
        $itemThumbnail = $pq_block->find($this->imageAddressSP)->attr('src');

        //We parse link to array and then gather only required parts.
        //For example, usually links from OLX ends with smth like "#1b78919f15;promoted" and we don't need such trash.
        $itemLink = $pq_block->find($this->titleLinkSP)->attr('href');
        $linkParts = parse_url($itemLink);
        $itemLinkClean = $linkParts['scheme'].'://'.$linkParts['host'].$linkParts['path']; //Link now clean.

        //Get price. At the moment this is 'raw' price, with number and currency ID, like 5 000 грн. We'll parse it later:
        $itemPrice = $pq_block->find($this->priceSP)->text();

        $collectedItems[$cii]['title'] = trim($itemTitle);
        $collectedItems[$cii]['thumbnail'] = $itemThumbnail;
        $collectedItems[$cii]['link'] = $itemLinkClean;
        $collectedItems[$cii]['price_raw'] = trim($itemPrice);
        $collectedItems[$cii]['price_value'] = (string) preg_replace('/\D/', '', $itemPrice); //Filter out ANYTHING THAT NOT a digit.
        $price_raw_nospaces = (string) preg_replace('/\s/', '', $itemPrice);
        $collectedItems[$cii]['price_currency'] = (string) preg_replace('/(\d+)([^0-9]+)(\.)$/', '$2', $price_raw_nospaces); //Filter out digits, leave only currency ID.
        $cii++;
      }

      //Get link to next search results page:
      $nextPageLink = $doc->find($this->nextPageLinkSP)->attr('href');

      phpQuery::unloadDocuments($doc);

      $url = $nextPageLink;
      $pgCount++;

      //If digging limit not reached ($pgCount < $this->pageLimit),
      //or $this->pageLimit set to 0 (Unlimited) -> set $willContinue flag to TRUE:
      $willContinue = (($pgCount < $this->pageLimit) || ($this->pageLimit === 0));

    } while (($url !== '') && $willContinue);

    return $collectedItems;
  }
}

class bsp_ItemSniper extends olx_ItemSniper {
  public function __construct($URL, $pageLimit, string $advertBlockSP = 'div.message > div.wrap', string $titleSP = 'div.content > div.title > a', string $titleLinkSP = 'div.content > div.title > a', string $priceSP = 'div.price', string $imageAddressSP = 'img.img-responsive', string $nextPageLinkSP = 'ul.pagination > li.next > a') {
    parent::__construct($URL, $pageLimit, $advertBlockSP, $titleSP, $titleLinkSP, $priceSP, $imageAddressSP, $nextPageLinkSP);
  }
}