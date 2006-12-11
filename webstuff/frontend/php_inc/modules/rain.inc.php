<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */


include_once($path."php_inc/connection.inc.php");

/* Rain-Klasse fuer das Rain-Modul */
class Rain{

  var $nowHour;		/* Regen in der momentanen Stunde */
  var $nowDay;		/* Regen in heute */			
  var $nowMonth;	/* Regen diesen Monat */
  var $nowYear;		/* Regen dies Jahr */
  var $nowAll;		/* Regen seit Messbeginn */
  var $maxHourData;	/* Stunde mit dem meistem Regen */
  var $maxDayData;	/* Tag mit dem meistem Regen */
  var $maxMonthData;	/* Monat mit dem meistem Regen */
  var $maxYearData;	/* Jahr mit dem meistem Regen */

  /* Konstruktor, Holt die Werte aus der Datenbank und fuellt die Variablen damit */
  function Rain($sensId, & $connection){

    /* Tabelle des Sensors bestimmen */
    $tableQuery  = "SELECT tabelle FROM sensoren, typen WHERE sensoren.id=".$sensId." AND typen.typ = sensoren.typ";
    $table       = $connection->fetchQueryResultLine($tableQuery);
    $table 	 = trim($table['tabelle']);

    /* Aktuelle Werte holen */
    $this->nowHour  = $this->_getNowValues($sensId, & $connection, $table, "1 hours");
    $this->nowDay   = $this->_getNowValues($sensId, & $connection, $table, "1 days");
    $this->nowMonth = $this->_getNowValues($sensId, & $connection, $table, "1 months");
    $this->nowYear  = $this->_getNowValues($sensId, & $connection, $table, "1 years");
    $this->nowAll   = $connection->fetchQueryResultLine("SELECT sum(count) as rain FROM ".$table." WHERE sens_id=".$sensId);
     
    /* Maximale Werte holen */
    $this->maxHourData =  $this->_getMaxValues($sensId, & $connection, $table, "hour", "DD.MM.YYYY  HH24:MI");
    $this->maxDayData =  $this->_getMaxValues($sensId, & $connection, $table, "day", "DD.MM.YYYY");
    $this->maxMonthData =  $this->_getMaxValues($sensId, & $connection, $table, "month", "MM.YYYY");
    $this->maxYearData =  $this->_getMaxValues($sensId, & $connection, $table, "year", "YYYY");
  }

  /* Momentane Werte aus der Datenbank holen */
  function _getNowValues($sensId, & $connection, $table, $interval){
    $result = $connection->fetchQueryResultLine("SELECT sum(count) as rain FROM ".$table." WHERE sens_id=".$sensId." AND timestamp>(current_timestamp - INTERVAL '".$interval."')");
    return $result['rain'];
  }

  /* Maximal gemessene Werte aus der Datenbank holen */
  function _getMaxValues($sensId, & $connection, $table, $unit, $dateFormat){ // unit = hour, minute, ...
    return $connection->fetchQueryResultLine("SELECT to_char(ts, '".$dateFormat."') as date, val FROM ".$table."_".$unit." WHERE sens_id=".$sensId." AND val=(SELECT max(val) FROM ".$table."_".$unit." WHERE sens_id=".$sensId.")");
  }


  /* --- Funktionen, die die Werte fuer die Ausgabe zurueckgeben --- */
  function get_now_hour(){
    return round($this->nowHour * 0.001,3);
  }

  function get_now_day(){
    return round($this->nowDay * 0.001,3);
  }

  function get_now_month(){
    return round($this->nowMonth * 0.001,3);
  }

  function get_now_year(){
    return round($this->nowYear * 0.001,3);
  }

  function get_now_all(){
    return round($this->nowAll['rain'] * 0.001,3);
  }

  function get_max_hour_val(){
    return round($this->maxHourData['val'] * 0.001, 3);
  }

  function get_max_hour_date(){
    return $this->maxHourData['date'];
  }

  function get_max_day_val(){
    return round($this->maxDayData['val'] * 0.001, 3);
  }

  function get_max_day_date(){
    return $this->maxDayData['date'];
  }

  function get_max_month_val(){
    return round($this->maxMonthData['val'] * 0.001, 3);
  }

  function get_max_month_date(){
    return $this->maxMonthData['date'];
  }

  function get_max_year_val(){
    return round($this->maxYearData['val'] * 0.001, 3);
  }

  function get_max_year_date(){
    return $this->maxYearData['date'];
  }

}
?>
