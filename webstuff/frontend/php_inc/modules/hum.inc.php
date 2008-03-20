<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */

/* Hum-Klasse, Holt Werte zur Luftfeuchtigkeit */
class Hum{

  var $nowHum;		/* Momentane Luftfeuchtigkeit */
  var $nowDate;		/* datum des letzten Messvorgangs */
  var $avVal    = "nc";	/* Durchschnittswert */
  var $avInter  = "nc";	/* Interval des Durchschnittswertes */
  var $minHum   = "nc";	/* Minimale Luftfeuchtigkeit */
  var $minDate  = "nc";	/* Datum, wann die Minimale Luftfeuchtigkeit gemessen wurde */
  var $maxHum   = "nc";	/* Maximale Luftfeuchtigkeit */
  var $maxDate  = "nc";	/* Datum, wann die Max. Luftfeuchtigkeit. gemessen wurde */
  var $changing = "nc";	/* Tendenz */
  var $connection;
  var $sensId;
  var $table;

  /* Konstruktor */
  function Hum($sensId, & $connection, $table){
    $this->connection = &$connection;
    $this->sensId     = $sensId;
    $this->table      = $table;
    $this->_fetchHumData();
  }

  /* Funktion, die die Klasse mit den Weten initialisiert */
  function _fetchHumData(){
    
    /* Aktuelle Luftfeuchtigkeit bestimmen */
    $nowQuery    = "SELECT hum, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$this->table." WHERE sens_id=".$this->sensId." ORDER BY timestamp DESC LIMIT 1";
    $nowData     = $this->connection->fetchQueryResultLine($nowQuery);
    
    /* Bestimmte Werte den Klassenvariablen zuordnen */
    $this->nowHum = $nowData['hum'];
    $this->nowDate = $nowData['text_timestamp'];
  }

  function _fetchMinMax(){
    $Data = Sensor::get_sensor_extrema($this->sensId, &$this->connection, "hum");
    $this->minHum = $Data['min'];
    $this->maxHum = $Data['max'];
  }

  function _fetchMinMaxDate(){
    if($this->maxHum == "nc" || $this->minHum == "nc"){
      $this->_fetchMinMax();
    }
    $Query    = "SELECT to_char(max(timestamp), 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$this->table." WHERE sens_id=".$this->sensId." AND hum=".$this->maxHum." OR hum=".$this->minHum." GROUP BY hum ORDER BY hum ASC LIMIT 2";
    $Data     = $this->connection->fetchQueryResultSet($Query);
    $this->minDate = $Data[0]['text_timestamp'];
    $this->maxDate = $Data[1]['text_timestamp'];
  }
 

  /* momentanen Durchschnittswert bestimmen */
  function _fetchAverage(){
    $Data = Sensor::get_sensor_average($this->sensId, &$this->connection, "hum");
    /* Werte den Klassenvariablen zuordnen */
    $this->avVal   = $Data['average'];
    $this->avInter = $Data['interval'];
  }
 
  /* Bestimmt die Tendenz */
  function _fetchMoving(){
    $shortAvData = Sensor::get_sensor_extrema($this->sensId, &$this->connection, "hum", 15); /* Durchschnitt der letzten 15 minuten */
    $longAvData = Sensor::get_sensor_extrema($this->sensId, &$this->connection, "hum", 120); /* Durchschnitt der letzten 120 Minuten */
    if($shortAvData['count'] < 1 || $longAvData['count'] < 2){				/* Wenn in den letzten 5 minuten kein Wert kam oder in den letzten 120 min weniger als 3 Werte kamen */
      $this->changing = "Berechnung momentan nicht moeglich";				/* Dann ausgeben, dass momentan nichts berechnet werden kann */
      return;										/* und aus der Funktion huepfen */
    }
    $changing = $shortAvData['average'] - $longAvData['average'];			/* Aenderung berechnen */
    if($changing > 0){									/* Wenn Aenderung positiv */
      $this->changing = "steigend (+ ".abs(round($changing * 0.1, 1))."%)";			/* dann steigende Tendenz ausgeben */
    } elseif($changing < 0) {								/* wenn Negativ */
      $this->changing = "fallend (- ".abs(round($changing * 0.1, 1))."%)";			/* Fallende Tendenz ausgeben */
    } else {										/* an sonsten */
      $this->changing = "gleichbleibend (&plusmn; 0%)";					/* sagen, das es gleich geblieben ist */
    }
    return;
  }

  /* --- Funktionen, die aufgerufen werden um die geholeten Werte auszugeben --- */
  function get_now_val(){
    return $this->nowHum;
  }

  function get_now_date(){
    return $this->nowDate;
  }

  function get_av_value(){
    if($this->avVal == "nc")
      $this->_fetchAverage();
    return round($this->avVal, 1);
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
    if($this->maxHum == "nc"){
      $this->_fetchMinMax();
    }
    return $this->maxHum;
  }

  function get_max_date(){
    if($this->minDate == "nc"){
      $this->_fetchMinMaxDate();
    }
    return $this->maxDate;
  }

  function get_min_val(){
    if($this->minHum == "nc"){
      $this->_fetchMinMax();
    }
    return $this->minHum;
  }

  function get_min_date(){
    if($this->maxDate == "nc"){
      $this->_fetchMinMaxDate();
    }
    return $this->minDate;
  }

}
?>
