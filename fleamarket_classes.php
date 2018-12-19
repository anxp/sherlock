<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2018-12-17
 * Time: 22:01
 */
class FleaMarket {
  protected static $marketName = '';
  protected static $baseURL = '';
  protected static $searchURL = '';
  protected static $wordsGlue = '';
  protected static $suffixDescrChk = '';

  //We don't want to allow making object directly of parent class, because resource-dependent parameters are not correctly set.
  protected function __construct() {
  }

  /**
   * @return string
   */
  public static function getMarketName() {
    return static::$marketName;
  }

  /**
   * @return string
   */
  public static function getBaseURL() {
    return static::$baseURL;
  }

  /**
   * @return string
   */
  public static function getSearchURL() {
    return static::$searchURL;
  }

  /**
   * @return string
   */
  public static function getWordsGlue() {
    return static::$wordsGlue;
  }

  /**
   * @return string
   */
  public static function getSuffixDescrChk() {
    return static::$suffixDescrChk;
  }
}

class OlxMarket extends FleaMarket {
  protected static $marketName = 'OLX';
  protected static $baseURL = 'https://www.olx.ua';
  protected static $searchURL = 'https://www.olx.ua/list/q-';
  protected static $wordsGlue = '-';
  protected static $suffixDescrChk = '/?search[description]=1';

  public function __construct() {
    parent::__construct();
  }
}

class BesplatkaMarket extends FleaMarket {
  protected static $marketName = 'Besplatka';
  protected static $baseURL = 'https://besplatka.ua';
  protected static $searchURL = 'https://besplatka.ua/all/q-';
  protected static $wordsGlue = '+';
  protected static $suffixDescrChk = '';

  public function __construct() {
    parent::__construct();
  }
}

class SkylotsMarket extends FleaMarket {
  protected static $marketName = 'SkyLots';
  protected static $baseURL = 'https://skylots.org';
  protected static $searchURL = 'https://skylots.org/search.php?search=';
  protected static $wordsGlue = '+';
  protected static $suffixDescrChk = '&desc_check=1';

  public function __construct() {
    parent::__construct();
  }
}
