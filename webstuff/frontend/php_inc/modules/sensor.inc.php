<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */


include_once($path."php_inc/connection.inc.php");


/* Klasse, Die Daten ueben die Einzelnen Sensoren bereitstellt */
class Sensor{
  
  var $id;		/* Die ID des Sensors */
  var $location;	/* Der Standort des Sensors */
  var $descr;		/* Beschreibung des Sensors */
  var $typ;		/* Typ des Sensors */
  var $address;		/* Addresse des Sensors */

  /* Konstruktor, Initialisiert die Klasse mit den Werten */
  function Sensor($sensId, & $connection){
    $this->_fetchSensorData($sensId, &$connection);
  }

  /* Holt die Daten ueber den Sensor aus der Datenbank */
  function _fetchSensorData($sensId, &$connection){
    $query  = "SELECT * FROM sensoren WHERE id=".$sensId;
    $data   = $connection->fetchQueryResultLine($query);
    
    $this->id       = $data['id'];
    $this->location = $data['standort'];
    $this->descr    = $data['beschreibung'];
    $this->typ      = $data['typ'];
    $this->address  = $data['addresse'];
  }

/* --- Funktionen, die die Einzelnen Eigenschaften des Sensors zurueckgeben --- */
  function get_id(){
    return $this->id;
  }

  function get_location(){
    return $this->location;
  }

  function get_description(){
    return $this->descr;
  }

  function get_type(){
    return $this->typ;
  }

  function get_address(){
    return $this->address;
  }
}

?>
