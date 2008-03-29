<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */

class Press{

  var $nowPress;		/* Momentaner Luftdruck */
  var $nowDate;			/* datum des letzten Messvorgangs */
  var $avVal      = "nc";	/* Durchschnittswert */
  var $avInter    = "nc";	/* Interval des Durchschnittswertes */
  var $minPress   = "nc";	/* Minimaler Luftdruck */
  var $minDate    = "nc";	/* Datum, wann der Minimale Luftdruck gemessen wurde */
  var $maxPress   = "nc";	/* Maximale Luftdruck */
  var $maxDate    = "nc";	/* Datum, wann der Max. Luftdruck gemessen wurde */
  var $changing   = "nc";	/* Tendenz */
  var $connection;
  var $sensId;
  var $table;

  /* Konstruktor */
  function Press($sensId, & $connection, $table){
    $this->connection = &$connection;
    $this->sensId     = $sensId;
    $this->table      = $table;
    $this->_fetchPressData();
  }

  /* Funktion, die die Klasse mit den Weten initialisiert */
  function _fetchPressData(){

    $nowData = null;
    if (($nowData = Cacher::getCache("PressNow_ID_".$this->sensId, 3)) == false){
      /* Aktuelle Luftdruck bestimmen */
      $nowQuery    = "SELECT press, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$this->table." WHERE sens_id=".$this->sensId." ORDER BY timestamp DESC LIMIT 1";
      $nowData     = $this->connection->fetchQueryResultLine($nowQuery);
      Cacher::setCache("PressNow_ID_".$this->sensId, $nowData);
    }
    
    /* Bestimmte Werte den Klassenvariablen zuordnen */
    $this->nowPress = $nowData['press'];
    $this->nowDate = $nowData['text_timestamp'];
  }

  function _fetchMinMax(){
    $Data = Sensor::get_sensor_extrema($this->sensId, &$this->connection, "press");
    $this->minPress = $Data['min'];
    $this->maxPress = $Data['max'];
  }

  function _fetchMinMaxDate(){
    if($this->maxPress == "nc" || $this->minPress == "nc"){
      $this->_fetchMinMax();
    }
    $Data = null;
    if (($Data = Cacher::getCache("PressExtremDate_ID_".$this->sensId."_MINMAX_".$this->maxTemp."_".$this->minTemp, 130)) == false){
      $Query    = "SELECT to_char(max(timestamp), 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$this->table." WHERE sens_id=".$this->sensId." AND press=".$this->maxPress." OR press=".$this->minPress." GROUP BY press ORDER BY press ASC LIMIT 2";
      $Data     = $this->connection->fetchQueryResultSet($Query);
      Cacher::setCache("PressExtremDate_ID_".$this->sensId."_MINMAX_".$this->maxTemp."_".$this->minTemp, $Data);
    }
    $this->minDate = $Data[0]['text_timestamp'];
    $this->maxDate = $Data[1]['text_timestamp'];
  }
 

  /* momentanen Durchschnittswert bestimmen */
  function _fetchAverage(){
    $Data = Sensor::get_sensor_average($this->sensId, &$this->connection, "press");
    /* Werte den Klassenvariablen zuordnen */
    $this->avVal   = $Data['average'];
    $this->avInter = $Data['interval'];
  }
 
  /* Bestimmt die Tendenz */
  function _fetchMoving(){
    $shortAvData = Sensor::get_sensor_extrema($this->sensId, &$this->connection, "press", 15); /* Durchschnitt der letzten 15 minuten */
    $longAvData = Sensor::get_sensor_extrema($this->sensId, &$this->connection, "press", 120); /* Durchschnitt der letzten 120 Minuten */
    if($shortAvData['count'] < 1 || $longAvData['count'] < 2){				/* Wenn in den letzten 5 minuten kein Wert kam oder in den letzten 120 min weniger als 3 Werte kamen */
      $this->changing = "Berechnung momentan nicht moeglich";				/* Dann ausgeben, dass momentan nichts berechnet werden kann */
      return;										/* und aus der Funktion huepfen */
    }
    $changing = $shortAvData['average'] - $longAvData['average'];			/* Aenderung berechnen */
    if($changing > 0){									/* Wenn Aenderung positiv */
      $this->changing = "steigend (+ ".abs(round($changing * 0.1, 1))." hPa)";			/* dann steigende Tendenz ausgeben */
    } elseif($changing < 0) {								/* wenn Negativ */
      $this->changing = "fallend (- ".abs(round($changing * 0.1, 1))." hPa)";			/* Fallende Tendenz ausgeben */
    } else {										/* an sonsten */
      $this->changing = "gleichbleibend (&plusmn; 0 hPa)";				/* sagen, das es gleich geblieben ist */
    }
    return;
  }

  /* --- Funktionen, die aufgerufen werden um die geholeten Werte auszugeben --- */
  function get_now_val(){
    return $this->nowPress;
  }

  function get_now_date(){
    return $this->nowDate;
  }

  function get_av_value(){
    if($this->avVal == "nc")
      $this->_fetchAverage();
    return round($this->avVal,1);
  }

  function get_av_interval(){
    if($this->avInter == "nc")
      $this->_fetchAverage();
    return $this->avInter;
  }

  function get_changing(){
    if($this->changing == "nc")
      $this->_fetchMoving();
    return $this->changing;
  }

  function get_max_val(){
    if($this->maxPress == "nc"){
      $this->_fetchMinMax();
    }
    return $this->maxPress;
  }

  function get_max_date(){
    if($this->minDate == "nc"){
      $this->_fetchMinMaxDate();
    }
    return $this->maxDate;
  }

  function get_min_val(){
    if($this->minPress == "nc"){
      $this->_fetchMinMax();
    }
    return $this->minPress;
  }

  function get_min_date(){
    if($this->maxDate == "nc"){
      $this->_fetchMinMaxDate();
    }
    return $this->minDate;
  }

}
?>
