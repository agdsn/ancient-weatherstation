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

    /* Aktuelle Luftdruck bestimmen */
    $nowQuery    = "SELECT press, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$this->table." WHERE sens_id=".$this->sensId." ORDER BY timestamp DESC LIMIT 1";
    $nowData     = $this->connection->fetchQueryResultLine($nowQuery);
    
    /* Bestimmte Werte den Klassenvariablen zuordnen */
    $this->nowPress = $nowData['press'];
    $this->nowDate = $nowData['text_timestamp'];
  }

  function _fetchMinMax(){
    $Query    = "SELECT max(press) as max, min(press) as min FROM ".$this->table." WHERE sens_id=".$this->sensId."";
    $Data     = $this->connection->fetchQueryResultLine($Query);
    $this->minPress = $Data['min'];
    $this->maxPress = $Data['max'];
  }

  function _fetchMinMaxDate(){
    if($this->maxHum == "nc" || $this->minHum == "nc"){
      $this->_fetchMinMax();
    }
    $Query    = "SELECT to_char(max(timestamp), 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$this->table." WHERE sens_id=".$this->sensId." AND press=".$this->maxPress." OR press=".$this->minPress." GROUP BY press ORDER BY press ASC LIMIT 2";
    $Data     = $this->connection->fetchQueryResultSet($Query);
    $this->minDate = $Data[0]['text_timestamp'];
    $this->maxDate = $Data[1]['text_timestamp'];
  }
 
  /* liefert den Durchschnittswert in einem bestimmtem Interval */
  function _getAverage($interval){
    $avQuery     = "SELECT avg(press) as average, count(press) as count  FROM ".$this->table." WHERE sens_id=".$this->sensId." AND timestamp>(current_timestamp - INTERVAL '".$interval."')";
    $avData      = $this->connection->fetchQueryResultLine($avQuery);
    return $avData;
  }

  /* momentanen Durchschnittswert bestimmen */
  function _fetchAverage(){
    $avData = array('average'=>0, 'count'=>0);						/* Array initialisieren */
    $i = 1;										/* Laufvariable */
    while($avData['count']<5){								/* Schleife prueft, in welchem Interval 5 Werte zusammenkommen */ 
      $i++;										/* Laufvariable erhoehen */
      $avData = $this->_getAverage(($i*(Config::getAvInterval()))." minutes");	/* Holt Werte mit gegebenem Interval */
    }

    /* Werte den Klassenvariablen zuordnen */
    $this->avVal   = $avData['average'];
    $this->avInter = $i*(Config::getAvInterval());
  }
 
  /* Bestimmt die Tendenz */
  function _fetchMoving(){
    $shortAvData = $this->_getAverage("15 minutes");					/* Durchschnitt der letzten 15 minuten */
    $longAvData = $this->_getAverage("120 minutes");					/* Durchschnitt der letzten 120 Minuten */
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
