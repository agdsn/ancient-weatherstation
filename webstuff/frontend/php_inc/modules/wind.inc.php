<?
class Wind{

  var $nowWind;		/* Momentaner Wind */
  var $nowDir;		/* Momentane Windrichtung */
  var $nowDate;		/* datum des letzten Messvorgangs */
  var $avVal;		/* Durchschnittswert */
  var $avInter;		/* Interval des Durchschnittswertes */
  var $maxWind;		/* Maximale Wind */
  var $maxDir;		/* Windrichtung bei max. Windgeschw. */
  var $maxDate;		/* Datum, wann der Max. Wind gemessen wurde */
  var $changing;	/* Tendenz */

  /* Konstruktor */
  function Wind($sensId, & $connection){
    $this->_fetchWindData($sensId, &$connection);
  }

  /* Funktion, die die Klasse mit den Weten initialisiert */
  function _fetchWindData($sensId, &$connection){

    /* Tabelle des Sensors bestimmen */
    $tableQuery  = "SELECT tabelle FROM sensoren, typen WHERE sensoren.id=".$sensId." AND typen.typ = sensoren.typ";
    $table       = $connection->fetchQueryResultLine($tableQuery);
    
    /* Aktuelle Wind bestimmen */
    $nowQuery    = "SELECT geschw as wind, richt as dir, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as timestamp FROM ".$table['tabelle']." WHERE timestamp=(select max(timestamp) from ".$table['tabelle']." where sens_id=".$sensId.")";
    $nowData     = $connection->fetchQueryResultLine($nowQuery);
    
    /* Max und Min-Werte bestimmen */
    $maxQuery    = "SELECT geschw as wind, richt as dir, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as timestamp FROM ".$table['tabelle']." WHERE sens_id=".$sensId." AND geschw=(SELECT max(geschw) FROM ".$table['tabelle']." WHERE sens_id=".$sensId.")";
    $maxData     = $connection->fetchQueryResultLine($maxQuery);
    
    /* Bestimmte Werte den Klassenvariablen zuordnen */
    $this->nowWind = $nowData['wind'];
    $this->nowDir  = $this->_calcDirection($nowData['dir']);
    $this->nowDate = $nowData['timestamp'];
    $this->maxWind = $maxData['wind'];
    $this->maxDir  = $this->_calcDirection($maxData['dir']);
    $this->maxDate = $maxData['timestamp'];

    /* Durchschnittswert bestimmen lassen */
    $this->_fetchAverage($sensId, $table['tabelle'], &$connection);

    /* Tendenz bestimmen lassen */
    $this->_fetchMoving($sensId, $table['tabelle'], &$connection);
  }

  function _calcDirection($deg){
    $part = 11.25;
    $val = floor($deg/$part);
    $i = 0;
    if($val == ++$i || $val == ++$i)
      return "N-N-O";
    elseif($val == ++$i || $val == ++$i)
      return "N-O";
    elseif($val == ++$i || $val == ++$i)
      return "O-N-O";
    elseif($val == ++$i || $val == ++$i)
      return "O";
    elseif($val == ++$i || $val == ++$i)
      return "O-S-O";
    elseif($val == ++$i || $val == ++$i)
      return "S-O";
    elseif($val == ++$i || $val == ++$i)
      return "S-S-O";
    elseif($val == ++$i || $val == ++$i)
      return "S";
    elseif($val == ++$i || $val == ++$i)
      return "S-S-W";
    elseif($val == ++$i || $val == ++$i)
      return "S-W";
    elseif($val == ++$i || $val == ++$i)
      return "W-S-W";
    elseif($val == ++$i || $val == ++$i)
      return "W";
    elseif($val == ++$i || $val == ++$i)
      return "W-N-W";
    elseif($val == ++$i || $val == ++$i)
      return "N-W";
    elseif($val == ++$i || $val == ++$i)
      return "N-N-W";
    else
      return "N";
  }

  function _genFullDirection($shortDir){
    switch ($shortDir){
      case "N": 
        return "Nord";
      case "N-N-O":
        return "Nord-Nord-Ost";
      case "N-O":
        return "Nord-Ost";
      case "O-N-O":
        return "Ost-Nord-Ost";
      case "O":
        return "Ost";
      case "O-S-O":
        return "Ost-S&uuml;d-Ost";
      case "S-O":
        return "S&uuml;d-Ost";
      case "S-S-O":
        return "S&uuml;d-S&uuml;d-Ost";
      case "S":
        return "S&uuml;d";
      case "S-S-W":
        return "S&uuml;d-S&uuml;d-West";
      case "S-W":
        return "S&uuml;d-West";
      case "W-S-W":
        return "West-S&uuml;d-West";
      case "W":
        return "West";
      case "W-N-W":
        return "West-Nord-West";
      case "N-W":
        return "Nord-West";
      case "N-N-W":
        return "Nord-Nord-West";
    }
  }

  /* liefert den Durchschnittswert in einem bestimmtem Interval */
  function _getAverage($sensId, $table, &$connection, $interval){
    $avQuery     = "SELECT (sum(geschw)/count(geschw)) as average, count(geschw) as count  FROM ".$table." WHERE sens_id=".$sensId." AND timestamp>(current_timestamp - INTERVAL '".$interval."')";
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
    return $this->nowWind;
  }

  function get_now_dir(){
    return $this->nowDir;
  }

  function get_now_dir_full(){
    return $this->_genFullDirection($this->nowDir);
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
    return $this->maxWind;
  }

  function get_max_dir(){
    return $this->maxDir;
  }

  function get_max_dir_full(){
    return $this->_genFullDirection($this->maxDir);
  }

  function get_max_date(){
    return $this->maxDate;
  }

}
?>
