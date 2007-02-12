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
  var $minPress;		/* Minimaler Luftdruck */
  var $minDate;			/* Datum, wann der Minimale Luftdruck gemessen wurde */
  var $maxPress;		/* Maximale Luftdruck */
  var $maxDate;			/* Datum, wann der Max. Luftdruck gemessen wurde */
  var $changing  = "nc";	/* Tendenz */
  var $connection;
  var $sensId;
  var $table;

  /* Konstruktor */
  function Press($sensId, & $connection){
    $this->_fetchPressData($sensId, &$connection);
  }

  /* Funktion, die die Klasse mit den Weten initialisiert */
  function _fetchPressData($sensId, &$connection){
    $this->connection = &$connection;
    $this->sensId     = $sensId;

    /* Tabelle des Sensors bestimmen */
    $tableQuery  = "SELECT tabelle FROM sensoren, typen WHERE sensoren.id=".$sensId." AND typen.typ = sensoren.typ";
    $table       = $connection->fetchQueryResultLine($tableQuery);
    $this->table = $table['tabelle'];
    
    /* Aktuelle Luftdruck bestimmen */
    $nowQuery    = "SELECT press, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$table['tabelle']." WHERE sens_id=".$sensId." ORDER BY timestamp DESC LIMIT 1";
    $nowData     = $connection->fetchQueryResultLine($nowQuery);
    
    /* Max und Min-Werte bestimmen */
    $maxQuery    = "SELECT press, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$table['tabelle']." WHERE sens_id=".$sensId." AND press=(SELECT max(press) FROM ".$table['tabelle']." WHERE sens_id=".$sensId.") ORDER BY timestamp DESC LIMIT 1";
    $maxData     = $connection->fetchQueryResultLine($maxQuery);
    $minQuery    = "SELECT press, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$table['tabelle']." WHERE sens_id=".$sensId." AND press=(SELECT min(press) FROM ".$table['tabelle']." WHERE sens_id=".$sensId.") ORDER BY timestamp DESC LIMIT 1";
    $minData     = $connection->fetchQueryResultLine($minQuery);
    
    /* Bestimmte Werte den Klassenvariablen zuordnen */
    $this->nowPress = $nowData['press'];
    $this->nowDate = $nowData['text_timestamp'];
    $this->maxPress = $maxData['press'];
    $this->maxDate = $maxData['text_timestamp'];
    $this->minPress = $minData['press'];
    $this->minDate = $minData['text_timestamp'];
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
    return $this->maxPress;
  }

  function get_max_date(){
    return $this->maxDate;
  }

  function get_min_val(){
    return $this->minPress;
  }

  function get_min_date(){
    return $this->minDate;
  }

}
?>
