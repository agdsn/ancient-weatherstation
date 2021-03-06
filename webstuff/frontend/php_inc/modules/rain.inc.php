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
  function Rain($sensId, & $connection, $table){
    $this->connection = &$connection;
    $this->sensId     = $sensId;
    $this->table = $table;
  }

  /* Momentane Werte aus der Datenbank holen */
  function _getNowValues($interval){
    if (($result = Cacher::getCache("NowRain_ID_".$this->sensId."_Interval_".$interval, 7)) == false){
      $result = $this->connection->fetchQueryResultLine("SELECT sum(count) as rain FROM ".$this->table." WHERE sens_id=".$this->sensId." AND timestamp>(select (current_timestamp - INTERVAL '".$interval."'))");
      Cacher::setCache("NowRain_ID_".$this->sensId."_Interval_".$interval, $result);
    }
    return $result['rain'];
  }

  /* Maximal gemessene Werte aus der Datenbank holen */
  function _getMaxValues($unit, $dateFormat){ // unit = hour, minute, ...
    $res = null;
    if (($res = Cacher::getCache("MaxRain_ID_".$this->sensId."_Unit_".$unit."_Format_".$dateFormat, 30)) == false ){
    $res =  $this->connection->fetchQueryResultLine("SELECT to_char(ts, '".$dateFormat."') as date, val FROM ".$this->table."_".$unit." WHERE sens_id=".$this->sensId." ORDER BY val DESC, ts DESC LIMIT 1");
    Cacher::setCache("MaxRain_ID_".$this->sensId."_Unit_".$unit."_Format_".$dateFormat, $res);
    }
    return $res;
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
      if (( $this->nowAll = Cacher::getCache("AllRainNow_ID_".$this->sensId, 20)) == false){
	$this->nowAll   = $this->connection->fetchQueryResultLine("SELECT sum(count) as rain FROM ".$this->table." WHERE sens_id=".$this->sensId);
	Cacher::setCache("AllRainNow_ID_".$this->sensId, $this->nowAll);
      }
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
