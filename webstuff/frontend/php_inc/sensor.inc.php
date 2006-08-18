<?

include_once("php_inc/connection.inc.php");


/* Klasse, Die Daten üben die Einzelnen Sensoren bereitstellt */
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

  /* Holt die Daten über den Sensor aus der Datenbank */
  function _fetchSensorData($sensId, &$connection){
    $query  = "SELECT * FROM sensoren WHERE id=".$sensId;
    $data   = $connection->fetchQueryResultLine($query);
    
    $this->id       = $data['id'];
    $this->location = $data['standort'];
    $this->descr    = $data['beschreibung'];
    $this->typ      = $data['typ'];
    $this->address  = $data['addresse'];
  }

/* --- Funktionen, die die Einzelnen Eigenschaften des Sensors zurückgeben --- */
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
