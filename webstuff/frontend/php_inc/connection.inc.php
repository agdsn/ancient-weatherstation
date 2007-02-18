<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */

include_once($path."php_inc/config.inc.php");

/* Connection-Klasse..ist fuer die Datenbankverbindung zustaendig */
class Connection{

  var $conn = NULL;	/* Verbindung */

  /* Konstruktor */
  function Connection(){
    $this->conn = NULL;
  }
  
  /* Verbindung herstellen (wenn noch net besteht)*/
  function _createConn(){
    //print $this->conn."<br>";
    if($this->conn === NULL){
      $this->conn = pg_pconnect(Config::getPgConnString())
        or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
    }
  }

  /* Verbindung schließen */
  function closeConn(){
    if($this->conn != NULL){
      pg_close($this->conn);  
      $this->conn = NULL;
    }
  }

  /* Eine Zeile holen */
  function fetchQueryResultLine($query){
     $this->_createConn();
     $result =  pg_query($this->conn, $query) 
       or die('Abfrage fehlgeschlagen: ' . pg_last_error(). "\n<br>\nquery: '".$query."'"); 
     $array = pg_fetch_assoc($result);
     //print_r($array);
     return $array;
  }

  /* mehrere Zeilen holen */
  function fetchQueryResultSet($query){
    $returnArray = array();
    $this->_createConn();
    $result =  pg_query($this->conn, $query)
      or die('Abfrage fehlgeschlagen: ' . pg_last_error(). "\n<br>\nquery: '".$query."'");
    while($array = pg_fetch_assoc($result))
      $returnArray[] = $array;
    return $returnArray;
  }

  /* Result roh zurueckgeben */
  function &getRawResult($query){
    $this->_createConn();
    $result =  pg_query($this->conn, $query)
      or die('Abfrage fehlgeschlagen: ' . pg_last_error(). "\n<br>\nquery: '".$query."'");
    return $result;
  }

}
?>
