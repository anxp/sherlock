<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2019-03-19
 * Time: 08:20
 */

class skl_MarketReference extends MarketReference implements iMarketReference {
  protected static $marketId = 'skl';
  protected static $marketName = 'SkyLots';
  protected static $domainName = 'skylots.org';
  protected static $searchPointOfEntry = 'search.php';
  protected static $wordsGlue = '+';

  public static function makeRequestURL(array $keyWords, int $priceFrom = null, int $priceTo = null, bool $checkDescription = false): string {
    $queryParameters = [];
    $queryParameters['search'] = implode(self::$wordsGlue, $keyWords);
    $queryParameters['desc_check'] = $checkDescription ? 1 : 0;

    $generatedQueryString = http_build_query($queryParameters);

    //               https://skylots.org  /                           search.php                  ?                        search=keyword_1+keyword_2&desc_check=1
    $fullQueryURL = (self::getBaseURL()).(self::URL_PARTS_SEPARATOR).(self::$searchPointOfEntry).(self::QUERY_IDENTIFIER).($generatedQueryString);
    return $fullQueryURL;
  }
}

//Example usage:
//echo skl_MarketReference::makeRequestURL(['Parker', 'перьевая', 'ручка'], null, null, true);