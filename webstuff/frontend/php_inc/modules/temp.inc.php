<?
class Temp{

  var $nowTemp;		/* Momentane Temparatur */
  var $nowDate;		/* datum des letzten Messvorgangs */
  var $avVal;		/* Durchschnittswert */
  var $avInter;		/* Interval des Durchschnittswertes */
  var $minTemp;		/* Minimale Temparatur */
  var $minDate;		/* Datum, wann die Minimale Temparatur gemessen wurde */
  var $maxTemp;		/* Maximale Temparatur */
  var $maxDate;		/* Datum, wann die Max. Temp. gemessen wurde */
  var $changing;	/* Tendenz */

  /* Konstruktor */
  function Temp($sensId, & $connection){
    $this->_fetchTempData($sensId, &$connection);
  }

  /* Funktion, die die Klasse mit den Weten initialisiert */
  function _fetchTempData($sensId, &$connection){

    /* Tabelle des Sensors bestimmen */
    $tableQuery  = "SELECT tabelle FROM sensoren, typen WHERE sensoren.id=".$sensId." AND typen.typ = sensoren.typ";
    $table       = $connection->fetchQueryResultLine($tableQuery);
    
    /* Aktuelle Temperatur bestimmen */
    $nowQuery    = "SELECT temp, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as timestamp FROM ".$table['tabelle']." WHERE timestamp=(select max(timestamp) from ".$table['tabelle']." where sens_id=".$sensId.")";
    $nowData     = $connection->fetchQueryResultLine($nowQuery);
    
    /* Max und Min-Werte bestimmen */
    $maxQuery    = "SELECT temp, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as timestamp FROM ".$table['tabelle']." WHERE sens_id=".$sensId." AND temp=(SELECT max(temp) FROM ".$table['tabelle']." WHERE sens_id=".$sensId.")";
    $maxData     = $connection->fetchQueryResultLine($maxQuery);
    $minQuery    = "SELECT temp, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as timestamp FROM ".$table['tabelle']." WHERE sens_id=".$sensId." AND temp=(SELECT min(temp) FROM ".$table['tabelle']." WHERE sens_id=".$sensId.")";
    $minData     = $connection->fetchQueryResultLine($minQuery);
    
    /* Bestimmte Werte den Klassenvariablen zuordnen */
    $this->nowTemp = $nowData['temp'];
    $this->nowDate = $nowData['timestamp'];
    $this->maxTemp = $maxData['temp'];
    $this->maxDate = $maxData['timestamp'];
    $this->minTemp = $minData['temp'];
    $this->minDate = $minData['timestamp'];
 
    /* Durchschnittswert bestimmen lassen */
    $this->_fetchAverage($sensId, $table['tabelle'], &$connection);

    /* Tendenz bestimmen lassen */
    $this->_fetchMoving($sensId, $table['tabelle'], &$connection);
  }

  /* liefert den Durchschnittswert in einem bestimmtem Interval */
  function _getAverage($sensId, $table, &$connection, $interval){
    $avQuery     = "SELECT (sum(temp)/count(temp)) as average, count(temp) as count  FROM ".$table." WHERE sens_id=".$sensId." AND timestamp>(current_timestamp - INTERVAL '".$interval."')";
    $avData      = $connection->fetchQueryResultLine($avQuery);
    return $avData;
  }

  /* momentanen Durchschnittswert bestimmen */
  function _fetchAverage($sensId, $table, &$connection){
    $avData = array('average'=>0, 'count'=>0);						/* Array initialisieren */
    $i = 1;										/* Laufvariable */
    while($avData['count']<5){								/* Schleife prüft, in welchem Interval 5 Werte zusammenkommen */ 
      $i++;										/* Laufvariable erhöhen */
      $avData = $this->_getAverage($sensId, $table, &$connection, ($i*15)." minutes");	/* Holt Werte mit gegebenem Interval */
    }

    /* Werte den Klassenvariablen zuordnen */
    $this->avVal   = $avData['average'];
    $this->avInter = $i*15;
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
      $this->changing = "steigend (+ ".abs($changing * 0.1)."&deg;C)";			/* dann steigende Tendenz ausgeben */
    } elseif($changing < 0) {								/* wenn Negativ */
      $this->changing = "fallend (- ".abs($changing * 0.1)."&deg;C)";			/* Fallende Tendenz ausgeben */
    } else {										/* an sonsten */
      $this->changing = "gleichbleibend (&plusmn; 0&deg;C)";				/* sagen, das es gleich geblieben ist */
    }
    return;
  }

  /* --- Funktionen, die aufgerufen werden um die geholeten Werte auszugeben --- */
  function get_now_val(){
    return $this->nowTemp * 0.1;
  }

  function get_now_date(){
    return $this->nowDate;
  }

  function get_av_value(){
    return $this->avVal * 0.1;
  }

  function get_av_interval(){
    return $this->avInter;
  }

  function get_changing(){
    return $this->changing;
  }

  function get_max_val(){
    return $this->maxTemp * 0.1;
  }

  function get_max_date(){
    return $this->maxDate;
  }

  function get_min_val(){
    return $this->minTemp * 0.1;
  }

  function get_min_date(){
    return $this->minDate;
  }

}
?>
