<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */


include_once($path."php_inc/connection.inc.php");
include_once($path."php_inc/cacher.inc.php");

$sensor_data = NULL;
$extrema_fields = array(
  0 => array('temp'),
  1 => array('temp', 'hum'),
  2 => array('count'),
  6 => array('intens'),
  5 => array('bight'),
  4 => array('temp', 'hum', 'press'),
  3 => array('geschw')
);
$sensor_extrema = array();
$sensor_average = array();

/* Klasse, Die Daten ueben die Einzelnen Sensoren bereitstellt */
class Sensor{
  
  var $id;		/* Die ID des Sensors */
  var $location;	/* Der Standort des Sensors */
  var $descr;		/* Beschreibung des Sensors */
  var $typ;		/* Typ des Sensors */
  var $address;		/* Addresse des Sensors */
  var $tabelle;         /* Tabelle in der die Werte des Sensors stehen */

  /* Konstruktor, Initialisiert die Klasse mit den Werten */
  function Sensor($sensId, & $connection){
    $data = $this->_fetchSensorData($sensId, &$connection);
    $this->id       = $data['id'];
    $this->location = $data['standort'];
    $this->descr    = $data['beschreibung'];
    $this->typ      = $data['typ'];
    $this->address  = $data['addresse'];
    $this->table    = $data['tabelle'];
  }

  /* Holt die Daten ueber den Sensor aus der Datenbank */
  function _fetchSensorData($sensId, &$connection){
    global $sensor_data;
    if ($sensor_data == NULL){
      if (($sensor_data = Cacher::getCache("SensorData", 20)) == false){
	$query  = "SELECT sensoren.*, typen.tabelle FROM sensoren, typen where typen.typ = sensoren.typ";
	$result = $connection->getRawResult($query);
	while ($array = pg_fetch_assoc($result)){
	  $sensor_data[$array['id']] = $array;
	}
	Cacher::setCache("SensorData", $sensor_data);
      }
    }
    return $sensor_data[$sensId];
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

  function get_table(){
    return $this->table;
  }

  /* Statisch, 
   * Holt Extrema (Min/Max)
   * Braucht: Sensor-Id, Connection, Das Feld fuer das die Extrema ausgegeben werden sollen */
  function get_sensor_extrema($sensId, &$connection, $field){
    global $extrema_fields, $sensor_extrema;
    if (! array_key_exists($sensId, $sensor_extrema)){
      if (($sensor_extrema[$sensId] = Cacher::getCache("SensorExtrema_ID_".$sensId, 10)) == false){
	$data = Sensor::_fetchSensorData($sensId, &$connection);
	$query = "SELECT ";
	for ($i = 0; $i < count($extrema_fields[$data['typ']]); $i++){
	  if ($i != 0)
	    $query .= ", ";
	  $query .= "min(".$extrema_fields[$data['typ']][$i].") AS min_".$extrema_fields[$data['typ']][$i].", ";
	  $query .= "max(".$extrema_fields[$data['typ']][$i].") AS max_".$extrema_fields[$data['typ']][$i]." ";
	}
	$query .= "FROM ".$data['tabelle']." WHERE sens_id = ".$sensId;
	$res = $connection->fetchQueryResultLine($query);
	$sensor_extrema[$sensId] = $res;
	Cacher::setCache("SensorExtrema_ID_".$sensId, $sensor_extrema[$sensId]);
      }
    } 
    return array('min' => $sensor_extrema[$sensId]['min_'.$field], 'max' => $sensor_extrema[$sensId]['max_'.$field]);
  }


  /* Statisch, 
   * Holt Durchschnittswerte 
   * Braucht: Sensor-Id, Connection, Das Feld fuer das die Extrema ausgegeben werden sollen, 
   * optional ein interval ueber das gemittelt werden soll */
  function get_sensor_average($sensId, &$connection, $field, $init_interval = 0){
    global $extrema_fields, $sensor_average;
    if ((!  array_key_exists($init_interval, $sensor_average)) || (! array_key_exists($sensId, $sensor_average[$init_interval]))){
      if (($sensor_average[$init_interval][$sensId] = Cacher::getCache("SensorAverage_ID_".$sensId."_Interval_".$init_interval, 10)) == false){
	$interval = $init_interval;
	$data = Sensor::_fetchSensorData($sensId, &$connection);
	$query = "SELECT count(".$extrema_fields[$data['typ']][0].") AS count, ";
	for ($i = 0; $i < count($extrema_fields[$data['typ']]); $i++){
	  if ($i != 0)
	    $query .= ", ";
	  $query .= "avg(".$extrema_fields[$data['typ']][$i].") AS ".$extrema_fields[$data['typ']][$i]." ";
	}
	$query .= "FROM ".$data['tabelle']." WHERE sens_id = ".$sensId." AND timestamp>(select(current_timestamp - INTERVAL '";
	$res = array('count' => 0, 'average' => 0);
	$m = 0;
	if ($interval == 0){
	  while ($res['count'] < 4) {
	    $m++;
	    $interval = $m*(Config::getAvInterval());
	    $res = $connection->fetchQueryResultLine($query.$interval." minutes'))");
	  } 
	} else {
	  $res = $connection->fetchQueryResultLine($query.$interval." minutes'))");
	}
	$res['interval'] = $interval;
	$sensor_average[$init_interval][$sensId] = $res;
	Cacher::setCache("SensorAverage_ID_".$sensId."_Interval_".$init_interval, $sensor_average[$init_interval][$sensId]);
      }
    } 
    return array('average' => $sensor_average[$init_interval][$sensId][$field], 'count' => $sensor_average[$init_interval][$sensId]['count'], 'interval' =>  $sensor_average[$init_interval][$sensId]['interval']);
  }
}

?>
