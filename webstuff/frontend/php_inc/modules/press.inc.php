<?
class Press{

  var $nowPress;		/* Momentaner Luftdruck */
  var $nowDate;		/* datum des letzten Messvorgangs */
  var $avVal;		/* Durchschnittswert */
  var $avInter;		/* Interval des Durchschnittswertes */
  var $minPress;		/* Minimaler Luftdruck */
  var $minDate;		/* Datum, wann der Minimale Luftdruck gemessen wurde */
  var $maxPress;		/* Maximale Luftdruck */
  var $maxDate;		/* Datum, wann der Max. Luftdruck gemessen wurde */
  var $changing;	/* Tendenz */

  /* Konstruktor */
  function Press($sensId, & $connection){
    $this->_fetchPressData($sensId, &$connection);
  }

  /* Funktion, die die Klasse mit den Weten initialisiert */
  function _fetchPressData($sensId, &$connection){

    /* Tabelle des Sensors bestimmen */
    $tableQuery  = "SELECT tabelle FROM sensoren, typen WHERE sensoren.id=".$sensId." AND typen.typ = sensoren.typ";
    $table       = $connection->fetchQueryResultLine($tableQuery);
    
    /* Aktuelle Luftdruck bestimmen */
    $nowQuery    = "SELECT press, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as timestamp FROM ".$table['tabelle']." WHERE timestamp=(select max(timestamp) from ".$table['tabelle']." where sens_id=".$sensId.")";
    $nowData     = $connection->fetchQueryResultLine($nowQuery);
    
    /* Max und Min-Werte bestimmen */
    $maxQuery    = "SELECT press, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as timestamp FROM ".$table['tabelle']." WHERE sens_id=".$sensId." AND press=(SELECT max(press) FROM ".$table['tabelle']." WHERE sens_id=".$sensId.")";
    $maxData     = $connection->fetchQueryResultLine($maxQuery);
    $minQuery    = "SELECT press, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as timestamp FROM ".$table['tabelle']." WHERE sens_id=".$sensId." AND press=(SELECT min(press) FROM ".$table['tabelle']." WHERE sens_id=".$sensId.")";
    $minData     = $connection->fetchQueryResultLine($minQuery);
    
    /* Bestimmte Werte den Klassenvariablen zuordnen */
    $this->nowPress = $nowData['press'];
    $this->nowDate = $nowData['timestamp'];
    $this->maxPress = $maxData['press'];
    $this->maxDate = $maxData['timestamp'];
    $this->minPress = $minData['press'];
    $this->minDate = $minData['timestamp'];
 
    /* Durchschnittswert bestimmen lassen */
    $this->_fetchAverage($sensId, $table['tabelle'], &$connection);

    /* Tendenz bestimmen lassen */
    $this->_fetchMoving($sensId, $table['tabelle'], &$connection);
  }

  /* liefert den Durchschnittswert in einem bestimmtem Interval */
  function _getAverage($sensId, $table, &$connection, $interval){
    $avQuery     = "SELECT (sum(press)/count(press)) as average, count(press) as count  FROM ".$table." WHERE sens_id=".$sensId." AND timestamp>(current_timestamp - INTERVAL '".$interval."')";
    $avData      = $connection->fetchQueryResultLine($avQuery);
    return $avData;
  }

  /* momentanen Durchschnittswert bestimmen */
  function _fetchAverage($sensId, $table, &$connection){
    $avData = array('average'=>0, 'count'=>0);						/* Array initialisieren */
    $i = 3;										/* Laufvariable */
    while($avData['count']<5){								/* Schleife prüft, in welchem Interval 5 Werte zusammenkommen */ 
      $avData = $this->_getAverage($sensId, $table, &$connection, ($i*10)." minutes");	/* Holt Werte mit gegebenem Interval */
      $i++;										/* Laufvariable erhöhen */
    }

    /* Werte den Klassenvariablen zuordnen */
    $this->avVal   = $avData['average'];
    $this->avInter = $i*10;
  }
 
  /* Bestimmt die Tendenz */
  function _fetchMoving($sensId, $table, &$connection){
    $shortAvData = $this->_getAverage($sensId, $table, &$connection, "15 minutes");	/* Durchschnitt der letzten 15 minuten */
    $longAvData = $this->_getAverage($sensId, $table, &$connection, "120 minutes");	/* Durchschnitt der letzten 120 Minuten */
    if($shortAvData['count'] < 1 || $longAvData['count'] < 2){				/* Wenn in den letzten 5 minuten kein Wert kam oder in den letzten 120 min weniger als 3 Werte kamen */
      $this->changing = "Berechnung momentan nicht möglich";				/* Dann ausgeben, dass momentan nichts berechnet werden kann */
      return;										/* und aus der Funktion huepfen */
    }
    $changing = $shortAvData['average'] - $longAvData['average'];			/* Aenderung berechnen */
    if($changing > 0){									/* Wenn Aenderung positiv */
      $this->changing = "steigend (+ ".abs($changing * 0.1)." hPa)";			/* dann steigende Tendenz ausgeben */
    } elseif($changing < 0) {								/* wenn Negativ */
      $this->changing = "fallend (- ".abs($changing * 0.1)." hPa)";			/* Fallende Tendenz ausgeben */
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
    return $this->avVal;
  }

  function get_av_interval(){
    return $this->avInter;
  }

  function get_changing(){
    return $this->changing;
  }

  function get_max_val(){
    return $this->maxPress;
  }

  function get_max_date(){
    return $this->maxDate;
  }

  function get_min_val(){
    return $this->minPress * 0.1;
  }

  function get_min_date(){
    return $this->minDate;
  }

}
?>
