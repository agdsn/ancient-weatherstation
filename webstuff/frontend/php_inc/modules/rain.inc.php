<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */


include_once($path."php_inc/connection.inc.php");

/* Rain-Klasse fuer das Rain-Modul */
class Rain{

  var $nowHour      = "nc";	/* Regen in der momentanen Stunde */
  var $nowDay	    = "nc";	/* Regen in heute */			
  var $nowMonth     = "nc";	/* Regen diesen Monat */
  var $nowYear      = "nc";	/* Regen dies Jahr */
  var $nowAll       = "nc";	/* Regen seit Messbeginn */
  var $maxHourData  = "nc";	/* Stunde mit dem meistem Regen */
  var $maxDayData   = "nc";	/* Tag mit dem meistem Regen */
  var $maxMonthData = "nc";	/* Monat mit dem meistem Regen */
  var $maxYearData  = "nc";	/* Jahr mit dem meistem Regen */
  var $connection;
  var $table;
  var $sensId;

  /* Konstruktor, Holt die Werte aus der Datenbank und fuellt die Variablen damit */
  function Rain($sensId, & $connection){
    $this->connection = &$connection;
    $this->sensId     = $sensId;

    /* Tabelle des Sensors bestimmen */
    $tableQuery  = "SELECT tabelle FROM sensoren, typen WHERE sensoren.id=".$sensId." AND typen.typ = sensoren.typ";
    $table       = $connection->fetchQueryResultLine($tableQuery);
    $table 	 = trim($table['tabelle']);
    $this->table = $table;

  }

  /* Momentane Werte aus der Datenbank holen */
  function _getNowValues($interval){
    $result = $this->connection->fetchQueryResultLine("SELECT sum(count) as rain FROM ".$this->table." WHERE sens_id=".$this->sensId." AND timestamp>(current_timestamp - INTERVAL '".$interval."')");
    return $result['rain'];
  }

  /* Maximal gemessene Werte aus der Datenbank holen */
  function _getMaxValues($unit, $dateFormat){ // unit = hour, minute, ...
    return $this->connection->fetchQueryResultLine("SELECT to_char(ts, '".$dateFormat."') as date, val FROM ".$this->table."_".$unit." WHERE sens_id=".$this->sensId." AND val=(SELECT max(val) FROM ".$this->table."_".$unit." WHERE sens_id=".$this->sensId.") ORDER BY ts DESC LIMIT 1");
  }


  /* --- Funktionen, die die Werte fuer die Ausgabe zurueckgeben --- */
  function get_now_hour(){
    if($this->nowHour == "nc");
      $this->nowHour  = $this->_getNowValues( "1 hours");
    return round($this->nowHour * 0.001,3);
  }

  function get_now_day(){
    if($this->nowDay == "nc");
      $this->nowDay   = $this->_getNowValues( "1 days");
    return round($this->nowDay * 0.001,3);
  }

  function get_now_month(){
    if($this->nowMonth == "nc");
      $this->nowMonth = $this->_getNowValues( "1 months");
    return round($this->nowMonth * 0.001,3);
  }

  function get_now_year(){
    if($this->nowYear == "nc");
      $this->nowYear  = $this->_getNowValues( "1 years");
    return round($this->nowYear * 0.001,3);
  }

  function get_now_all(){
    if($this->nowAll == "nc");
      $this->nowAll   = $this->connection->fetchQueryResultLine("SELECT sum(count) as rain FROM ".$this->table." WHERE sens_id=".$this->sensId);
    return round($this->nowAll['rain'] * 0.001,3);
  }

  function get_max_hour_val(){
    if($this->maxHourData == "nc")
      $this->maxHourData =  $this->_getMaxValues("hour", "DD.MM.YYYY  HH24:MI");
    return round($this->maxHourData['val'] * 0.001, 3);
  }

  function get_max_hour_date(){
    if($this->maxHourData == "nc")
      $this->maxHourData =  $this->_getMaxValues("hour", "DD.MM.YYYY  HH24:MI");
    return $this->maxHourData['date'];
  }

  function get_max_day_val(){
    if($this->maxDayData == "nc")
      $this->maxDayData =  $this->_getMaxValues("day", "DD.MM.YYYY");
    return round($this->maxDayData['val'] * 0.001, 3);
  }

  function get_max_day_date(){
    if($this->maxDayData == "nc")
      $this->maxDayData =  $this->_getMaxValues("day", "DD.MM.YYYY");
    return $this->maxDayData['date'];
  }

  function get_max_month_val(){
    if($this->maxMonthData == "nc")
      $this->maxMonthData =  $this->_getMaxValues("month", "MM.YYYY");
    return round($this->maxMonthData['val'] * 0.001, 3);
  }

  function get_max_month_date(){
    if($this->maxMonthData == "nc")
      $this->maxMonthData =  $this->_getMaxValues("month", "MM.YYYY");
    return $this->maxMonthData['date'];
  }

  function get_max_year_val(){
    if($this->maxYearData)
      $this->maxYearData =  $this->_getMaxValues("year", "YYYY");
    return round($this->maxYearData['val'] * 0.001, 3);
  }

  function get_max_year_date(){
    if($this->maxYearData)
      $this->maxYearData =  $this->_getMaxValues("year", "YYYY");
    return $this->maxYearData['date'];
  }

}
?>
