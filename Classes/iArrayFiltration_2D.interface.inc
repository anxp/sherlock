<?php
/**
 * Created by PhpStorm.
 * User: andrey
 * Date: 2019-03-25
 * Time: 08:05
 */

interface iArrayFiltration_2D {
  public static function selectUniqueSubArrays_byField(array $inputArray, string $fieldToCheck): array;
  public static function selectUniqueSubArrays_byTotalCompare(array $inputArray): array;
  public static function selectSubArrays_byFieldValueRange(array $inputArray, string $fieldToCheck, float $minValue, float $maxValue): array;
}
