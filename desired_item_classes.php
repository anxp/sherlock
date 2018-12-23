<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2018-12-20
 * Time: 08:09
 */

require_once 'phpQuery/phpQuery.php';

abstract class DesiredItem {
  //Just the most popular user agent, it can be overriden by setUserAgent() method:
  protected $userAgent = 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.110 Safari/537.36';

  //This value will be set to CURLOPT_CONNECTTIMEOUT. Can be overriden by setConnectionTimeout() method:
  protected $connectionTimeout = 10;

  //URL to fetch. We will set it up in constructor:
  protected $URL = '';

  //Class or ID of DIV block which contain one advertisement. By this sign we search by phpQuery in the captured page.
  protected $signOfAdvertDivBlock = '';

  public function __construct($URL, $signOfAdvertDivBlock = '') {
    $this->URL = $URL;
    $this->signOfAdvertDivBlock = $signOfAdvertDivBlock;
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
   */
  public function getSignOfAdvertDivBlock(): string {
    return $this->signOfAdvertDivBlock;
  }

  /**
   * @param string $signOfAdvertDivBlock
   */
  public function setSignOfAdvertDivBlock(string $signOfAdvertDivBlock): void {
    $this->signOfAdvertDivBlock = $signOfAdvertDivBlock;
  }

  /**
   * @return string
   */
  public function getWebpageHtml() {
    $ch = curl_init();

    curl_setopt_array($ch, array(
      CURLOPT_USERAGENT => $this->getUserAgent(),
      CURLOPT_CONNECTTIMEOUT => $this->getConnectionTimeout(),
      CURLOPT_RETURNTRANSFER => TRUE,
      CURLOPT_HEADER => FALSE,
      CURLOPT_FOLLOWLOCATION => TRUE, //Follow redirects
      CURLOPT_URL => $this->URL,
    ));

    //Get URL content in string:
    $html_string = curl_exec($ch);

    //Close handle to release resources
    curl_close($ch);

    return $html_string;
  }

  /**
   * @return array
   */
  abstract public function parseHtmlGetContent(&$resultData) : array; //Recursive function which parse page -> get second page -> parse it, and so on.
}
