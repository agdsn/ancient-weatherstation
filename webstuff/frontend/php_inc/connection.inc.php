<?
class Connection{

  var $conn = NULL;

  function Connection(){
    $this->conn = NULL;
  }

  function _createConn(){
    if($this->conn == NULL){
      $this->conn = pg_connect("host=141.30.228.39 dbname=wetter user=losinshi")
        or die('Verbindungsaufbau fehlgeschlagen: ' . pg_last_error());
    }
  }

  function closeConn(){
    if($this->conn != NULL){
      pg_close($this->conn);  
      $this->conn = NULL;
    }
  }

  function fetchQueryResultLine($query){
     $this->_createConn();
     $result =  pg_query($this->conn, $query) 
       or die('Abfrage fehlgeschlagen: ' . pg_last_error()); 
     $array = pg_fetch_assoc($result);
     //print_r($array);
     return $array;
  }

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
