<?php

date_default_timezone_set('America/Lima');
$servername = "127.0.0.1";
$username = "Usuario";
$password = "Contraseña";
$dbname = "u975775819_HuertoTomates";


$conn = new mysqli($servername,$username,$password,$dbname);

if($conn -> connect_error){
	die("Conexion fallida: ". $conn->connect_error);
}

$fecha = date("Ymd");

// Nombre de la tabla basada en la fecha
$nombre_tabla = "Datos_" . $fecha;

$sql_tabla = "CREATE TABLE IF NOT EXISTS $nombre_tabla (
    idKey INT(11) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    Fecha TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    Humedad1 FLOAT,
    Humedad2 FLOAT,
    Humedad3 FLOAT,
    Humedad4 FLOAT,
    Humedad5 FLOAT,
    Humedad6 FLOAT,
    Humedad7 FLOAT,
    PotenciaINA FLOAT,
    CorrienteINA FLOAT,
    NivelDeAgua FLOAT,
    Fertilizante FLOAT,
    Temperatura FLOAT
)";

if ($conn->query($sql_tabla) === TRUE) {
    //Tabla creada
} else {
    //echo "Error al crear la tabla: " . $conn->error;
}


if($_SERVER["REQUEST_METHOD"] == "POST"){
    echo "Dentro del metodo POST";

	$humedad1 = $_POST['humedad1'];
	$humedad2 = $_POST['humedad2'];
	$humedad3 = $_POST['humedad3'];
	$humedad4 = $_POST['humedad4'];
	$humedad5 = $_POST['humedad5'];
	$humedad6 = $_POST['humedad6'];
	$humedad7 = $_POST['humedad7'];
	$potenciaINA = $_POST['potenciaINA'];
	$corrienteINA = $_POST['corrienteINA'];
	$nivelDeAgua = $_POST['nivelDeAgua'];
	$fertilizante = $_POST['fertilizante'];
	$temperatura = $_POST['temperatura'];

    $sql = "";
    $sql2 = "";

    if($humedad1 == 0 && $humedad2 ==0 && $corriente_ina != 0 && $potencia_ina!=0){
        $sql = "INSERT INTO Datos_electricidad (PotenciaINA,CorrienteINA) VALUES ('$potenciaINA','$corrienteINA')";
    }elseif($humedad1 == 0 && $humedad2 ==0 && $corriente_ina == 0 && $potencia_ina==0){
        $sql = "";
    }
    else{
	    $sql = "INSERT INTO $nombre_tabla (Humedad1,Humedad2,Humedad3,Humedad4,Humedad5,Humedad6,Humedad7,PotenciaINA,CorrienteINA,NivelDeAgua,Fertilizante,Temperatura) VALUES ('$humedad1','$humedad2','$humedad3','$humedad4','$humedad5','$humedad6','$humedad7','$potenciaINA','$corrienteINA','$nivelDeAgua','$fertilizante','$temperatura')";
	    $sql2 = "INSERT INTO Datos_electricidad (PotenciaINA,CorrienteINA) VALUES ($potencia_ina','$corriente_ina')";
    }

    if ($conn->query($sql) === TRUE) {
        //Se creo una nueva fila
    } else {
        //echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
    if($conn->query($sql2) === TRUE){
        //Se creo una nueva fila
    } 
    
}

$conn ->close();
?>
