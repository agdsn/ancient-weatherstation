<?
include_once("php_inc/config.inc.php");

/* Connection-Klasse..ist für die Datenbankverbindung zuständig */
class Connection{

  var $conn = NULL;	/* Verbindung */

  /* Konstruktor */
  function Connection(){
    $this->conn = NULL;
  }
  
  /* Verbindung herstellen (wenn noch net besteht)*/
  function _createConn(){
    if($this->conn == NULL){
      $this->conn = pg_connect(Config::getPgConnString())
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
       or die('Abfrage fehlgeschlagen: ' . pg_last_error()); 
     $array = pg_fetch_assoc($result);
     //print_r($array);
     return $array;
  }

  /* mehrere Zeilen holen */
  function fetchQueryResultSet($query){
    $returnArray = array();
    $this->_createConn();
    $result =  pg_query($this->conn, $query)
      or die('Abfrage fehlgeschlagen: ' . pg_last_error());
    while($array = pg_fetch_assoc($result))
      array_push($returnArray, $array);
    return $returnArray;
  }

}
?>
