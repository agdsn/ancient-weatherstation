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
    $Query    = "SELECT max(hum) as max, min(hum) as min FROM ".$this->table." WHERE sens_id=".$this->sensId."";
    $Data     = $this->connection->fetchQueryResultLine($Query);
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
 
  /* liefert den Durchschnittswert in einem bestimmtem Interval */
  function _getAverage($interval){
    $avQuery     = "SELECT avg(hum) as average, count(hum) as count  FROM ".$this->table." WHERE sens_id=".$this->sensId." AND timestamp>(current_timestamp - INTERVAL '".$interval."')";
    $avData      = $this->connection->fetchQueryResultLine($avQuery);
    return $avData;
  }

  /* momentanen Durchschnittswert bestimmen */
  function _fetchAverage(){
    $avData = array('average'=>0, 'count'=>0);						/* Array initialisieren */
    $i = 1;										/* Laufvariable */
    while($avData['count']<5){								/* Schleife prueft, in welchem Interval 5 Werte zusammenkommen */ 
      $i++;										/* Laufvariable erhoehen */
      $avData = $this->_getAverage(($i*(Config::getAvInterval()))." minutes");		/* Holt Werte mit gegebenem Interval */
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
