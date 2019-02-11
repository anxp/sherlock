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

  protected function getItemTitle(phpQueryObject $phpQueryNode): string {
    return (trim($phpQueryNode->find($this->titleSP)->text()));
  }

  protected function getThumbnailAddress(phpQueryObject $phpQueryNode): string {
    $thumbAddress = ($phpQueryNode->find($this->imageAddressSP)->attr('src')) ? ($phpQueryNode->find($this->imageAddressSP)->attr('src')) : '';
    return $thumbAddress;
  }

  protected function getItemLink(phpQueryObject $phpQueryNode): string {
    //We parse link to array and then gather only required parts.
    //For example, usually links from OLX ends with smth like "#1b78919f15;promoted" and we don't need such trash.
    $itemLink = $phpQueryNode->find($this->titleLinkSP)->attr('href');
    $linkParts = parse_url($itemLink);
    $itemLinkClean = $linkParts['scheme'].'://'.$linkParts['host'].$linkParts['path']; //Link now clean.
    return $itemLinkClean;
  }

  protected function getItemPrice(phpQueryObject $phpQueryNode): array {
    //Get price. At the moment this is 'raw' price, with number and currency ID, like 5 000 грн. We'll parse it later:
    $itemPriceRaw = trim($phpQueryNode->find($this->priceSP)->text());
    $itemPrice = [];
    $itemPrice['price_value'] = (string) preg_replace('/\D/', '', $itemPriceRaw); //Filter out ANYTHING THAT NOT a digit.
    $itemPriceRawNoSpaces = (string) preg_replace('/\s/', '', $itemPriceRaw);
    $itemPrice['price_currency'] = (string) preg_replace('/(\d+)([^0-9]+)(\.)$/', '$2', $itemPriceRawNoSpaces); //Filter out digits, leave only currency ID.
    return $itemPrice;
  }

  protected function getNextPageLink(phpQueryObject $phpQueryNode): string {
    return ($phpQueryNode->find($this->nextPageLinkSP)->attr('href'));
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
        $itemTitle = $this->getItemTitle($pq_block);

        //Get address of attached thumbnail:
        $itemThumbnail = $this->getThumbnailAddress($pq_block);

        //Get link to page with full description of ad:
        $itemLink = $this->getItemLink($pq_block);

        //Get item price (array of two elements - price_value and price_currency):
        $itemPrice = $this->getItemPrice($pq_block);

        $collectedItems[$cii]['title'] = trim($itemTitle);
        $collectedItems[$cii]['thumbnail'] = $itemThumbnail;
        $collectedItems[$cii]['link'] = $itemLink;
        $collectedItems[$cii]['price_value'] = $itemPrice['price_value'];
        $collectedItems[$cii]['price_currency'] = $itemPrice['price_currency'];
        $cii++;
      }

      //Get link to next search results page:
      $nextPageLink = $this->getNextPageLink($doc);

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
  public function __construct($URL, $pageLimit, string $advertBlockSP = 'div.message > div.wrap', string $titleSP = 'div.content > div.title > a', string $titleLinkSP = 'div.content > div.title > a', string $priceSP = 'div.price', string $imageAddressSP = 'img.img-responsive', string $nextPageLinkSP = 'head > link.pag_params') {
    parent::__construct($URL, $pageLimit, $advertBlockSP, $titleSP, $titleLinkSP, $priceSP, $imageAddressSP, $nextPageLinkSP);
  }

  protected function getItemTitle(phpQueryObject $phpQueryNode): string {
    return (trim($phpQueryNode->find($this->titleSP)->text()));
  }

  protected function getThumbnailAddress(phpQueryObject $phpQueryNode): string {
    //Get address of attached thumbnail. We use value from attribute 'data-src', because 'src' attribute does not exists when raw html received!
    $thumbAddress = ($phpQueryNode->find($this->imageAddressSP)->attr('data-src')) ? ($phpQueryNode->find($this->imageAddressSP)->attr('data-src')) : '';
    return $thumbAddress;
  }

  protected function getItemLink(phpQueryObject $phpQueryNode): string {
    //All links at Besplatka are 'internal', so we need add 'https://besplatka.ua' at the begining of them:
    return ('https://besplatka.ua'.$phpQueryNode->find($this->titleLinkSP)->attr('href'));
  }

  protected function getItemPrice(phpQueryObject $phpQueryNode): array {
    //Get price. At the moment this is 'raw' price, with number and currency ID, like 5 000,50 грн. We'll parse it later:
    $itemPriceRaw = trim($phpQueryNode->find($this->priceSP)->text()); //price now is like 2 286,5 грн.
    $itemPriceRawNoSpaces = (string) preg_replace('/\s/', '', $itemPriceRaw); //price now is like 2286,5грн. or more common: 2189грн.

    $itemPrice = []; //Here we will store price - price_value and price_currency.

    //Check for ',':
    $commaPosition = strpos($itemPriceRawNoSpaces, ',');

    if ($commaPosition === FALSE) {
      //if comma was not found in string:
      $itemPrice['price_value'] = (string) preg_replace('/\D/', '', $itemPriceRaw); //Filter out ANYTHING THAT NOT a digit.
      $itemPrice['price_currency'] = (string) preg_replace('/(\d+)([^0-9]+)(\.)$/', '$2', $itemPriceRawNoSpaces); //Filter out digits, leave only currency ID.
    } else {
      //if comma WAS FOUND in string:
      $exploded = explode(',', $itemPriceRawNoSpaces);
      $itemPrice['price_value'] = (string) $exploded[0];
      $itemPrice['price_currency'] = (string) preg_replace('/(\d+)([^0-9]+)(\.)$/', '$2', $exploded[1]); //Filter out digits, leave only currency ID.
    }

    return $itemPrice;
  }

  protected function getNextPageLink(phpQueryObject $phpQueryNode): string {
    $internalURL = $phpQueryNode->find($this->nextPageLinkSP)->attr('href');
    //Construct real full path only if found "next page" link is NOT empty, else -> just return '' (empty string):
    $path = ($internalURL === '') ? '' : ('https://besplatka.ua'.$internalURL);
    return ($path);
  }
}
