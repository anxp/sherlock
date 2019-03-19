<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2019-03-19
 * Time: 08:19
 */

interface iMarketReference {
  const PROTOCOL = 'https://';
  const URL_PARTS_SEPARATOR = '/';
  const QUERY_IDENTIFIER = '?';
  public static function makeRequestURL(array $keyWords, int $priceFrom = null, int $priceTo = null, bool $checkDescription = false): string ;
  public static function getMarketId(): string;
  public static function getMarketName(): string;
  public static function getBaseURL(): string;
}

abstract class MarketReference implements iMarketReference {
  protected static $marketId = ''; //Three lowercase-letter id of the market
  protected static $marketName = ''; //Human-readable market name
  protected static $domainName = ''; //Domain name, like olx.ua without protocol and ending '/'

  abstract public static function makeRequestURL(array $keyWords, int $priceFrom = null, int $priceTo = null, bool $checkDescription = false): string ;

  public static function getMarketId(): string {
    return static::$marketId;
  }

  public static function getMarketName(): string {
    return static::$marketName;
  }

  public static function getBaseURL(): string {
    return (static::PROTOCOL).(static::$domainName);
  }
}