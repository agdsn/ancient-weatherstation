<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */

class Wind{

  var $nowWind;			/* Momentaner Wind */
  var $nowDir;			/* Momentane Windrichtung */
  var $nowDate;			/* datum des letzten Messvorgangs */
  var $avVal      = "nc";	/* Durchschnittswert */
  var $avInter    = "nc";	/* Interval des Durchschnittswertes */
  var $maxWind;			/* Maximale Wind */
  var $maxDir;			/* Windrichtung bei max. Windgeschw. */
  var $maxDate;			/* Datum, wann der Max. Wind gemessen wurde */
  var $changing   = "nc";	/* Tendenz */
  var $connection;
  var $sensId;
  var $table;

  /* Konstruktor */
  function Wind($sensId, & $connection, $table){
    $this->table = $table;
    $this->connection = &$connection;
    $this->sensId = $sensId;
    $this->_fetchWindData();
  }

  /* Funktion, die die Klasse mit den Weten initialisiert */
  function _fetchWindData(){

    $nowData = null;
    if (($nowData = Cacher::getCache("WindNow_ID_".$this->sensId, 3)) == false){
      /* Aktuelle Wind bestimmen */
      $nowQuery    = "SELECT geschw as wind, richt as dir, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$this->table." WHERE sens_id=".$this->sensId." ORDER BY timestamp DESC LIMIT 1";
      $nowData     = $this->connection->fetchQueryResultLine($nowQuery);
      Cacher::setCache("WindNow_ID_".$this->sensId, $nowData);
    }
    
    $maxData = null;
    if (($nowData = Cacher::getCache("WindMax_ID_".$this->sensId, 3)) == false){
      /* Max und Min-Werte bestimmen */
      $maxQuery    = "SELECT geschw as wind, richt as dir, to_char(timestamp, 'DD.MM.YYYY  HH24:MI') as text_timestamp FROM ".$this->table." WHERE sens_id=".$this->sensId." AND geschw=(SELECT max(geschw) FROM ".$this->table." WHERE sens_id=".$this->sensId.") ORDER BY timestamp DESC LIMIT 1";
      $maxData     = $this->connection->fetchQueryResultLine($maxQuery);
      Cacher::setCache("WindMax_ID_".$this->sensId, $maxData);
    }
    
    /* Bestimmte Werte den Klassenvariablen zuordnen */
    $this->nowWind = $nowData['wind'];
    $this->nowDir  = $this->_calcDirection($nowData['dir']);
    $this->nowDate = $nowData['text_timestamp'];
    $this->maxWind = $maxData['wind'];
    $this->maxDir  = $this->_calcDirection($maxData['dir']);
    $this->maxDate = $maxData['text_timestamp'];
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
  function _getAverage($interval){
    $avData = null;
    if(($avData = Cacher::getCache("WindAv_ID_".$this->sensId."_INTERVAL_".$interval, 10)) == false){
      $avQuery     = "SELECT avg(geschw) as average, count(geschw) as count  FROM ".$this->table." WHERE sens_id=".$this->sensId." AND timestamp>(select (current_timestamp - INTERVAL '".$interval."'))";
      $avData      = $this->connection->fetchQueryResultLine($avQuery);
      Cacher::setCache("WindAv_ID_".$this->sensId."_INTERVAL_".$interval, $avData);
    }
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
    $shortAvData = $this->_getAverage("15 minutes");	  				/* Durchschnitt der letzten 15 minuten */
    $longAvData = $this->_getAverage("120 minutes");					/* Durchschnitt der letzten 120 Minuten */
    if($shortAvData['count'] < 1 || $longAvData['count'] < 2){				/* Wenn in den letzten 5 minuten kein Wert kam oder in den letzten 120 min weniger als 3 Werte kamen */
      $this->changing = "Berechnung momentan nicht moeglich";				/* Dann ausgeben, dass momentan nichts berechnet werden kann */
      return;										/* und aus der Funktion huepfen */
    }
    $changing = $shortAvData['average'] - $longAvData['average'];			/* Aenderung berechnen */
    if($changing > 0){									/* Wenn Aenderung positiv */
      $this->changing = "steigend (+ ".abs(round($changing * 0.1, 1))." <sup>km</sup>/<sub>h</sub>)";			/* dann steigende Tendenz ausgeben */
    } elseif($changing < 0) {								/* wenn Negativ */
      $this->changing = "fallend (- ".abs(round($changing * 0.1, 1))." <sup>km</sup>/<sub>h</sub>)";			/* Fallende Tendenz ausgeben */
    } else {										/* an sonsten */
      $this->changing = "gleichbleibend (&plusmn; 0 <sup>km</sup>/<sub>h</sub>)";				/* sagen, das es gleich geblieben ist */
    }
    return;
  }

  /* --- Funktionen, die aufgerufen werden um die geholeten Werte auszugeben --- */
  function get_now_val(){
    return round($this->nowWind * 0.1, 1);
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
    if($this->avVal == "nc")
      $this->_fetchAverage();
    return round($this->avVal * 0.1, 1);
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
    return round($this->maxWind * 0.1, 1);
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
