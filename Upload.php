<?php

date_default_timezone_set('America/Lima');
$servername = "127.0.0.1";
$username = "Usuario";
$password = "Contraseña";
$dbname = "Nombre_base_de_datos";
echo "Se ejecuto php";
$conn = new mysqli($servername,$username,$password,$dbname);

if($conn -> connect_error){
	die("Conexion fallida: ". $conn->connect_error);
	echo "Fallo en la coneccion";
	
}else{
    echo "Coneccion exitosa";
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
    CorrienteACS FLOAT,
    NivelDeAgua FLOAT,
    Fertilizante FLOAT,
    Temperatura FLOAT
)";

if ($conn->query($sql_tabla) === TRUE) {
    echo "Tabla $nombre_tabla creada o ya existente.";
} else {
    echo "Error al crear la tabla: " . $conn->error;
}


if($_SERVER["REQUEST_METHOD"] == "POST"){
    echo "Dentro del metodo POST";
	//$data = $_POST["data"];
    //echo $data;
	//$data_array = explode("&",$data);
	$humedad1 = $_POST['humidity1'];
	$humedad2 = $_POST['humidity2'];
	$humedad3 = $_POST['humidity3'];
	$humedad4 = $_POST['humidity4'];
	$humedad5 = $_POST['humidity5'];
	$humedad6 = $_POST['humidity6'];
	$humedad7 = $_POST['humidity7'];
	$potencia_ina = $_POST['power_ina'];
	$corriente_ina = $_POST['current_ina'];
	$corriente_acs = $_POST['current_acs'];
	$agua = $_POST['water_level'];
	$fertilizante = $_POST['fertilizer'];
	$temperatura = $_POST['temperature'];
	//echo "Humedad= "  . $humedad;
//	list($humedad,$potencia,$agua,$fertilizante,$tempertura) = explode("&",$data);
    //echo $data;
    $sql = "";
    $sql2 = "";
    if($humedad1 == 0 && $humedad2 ==0 && $corriente_ina != 0 && $potencia_ina!=0){
        $sql = "INSERT INTO Datos_electricidad (PotenciaINA,CorrienteINA,CorrienteACS) VALUES ('$potencia_ina','$corriente_ina','$corriente_acs')";
    }elseif($humedad1 == 0 && $humedad2 ==0 && $corriente_ina == 0 && $potencia_ina==0){
        $sql = "";
    }
    else{
	    $sql = "INSERT INTO $nombre_tabla (Humedad1,Humedad2,Humedad3,Humedad4,Humedad5,Humedad6,Humedad7,PotenciaINA,CorrienteINA,CorrienteACS,NivelDeAgua,Fertilizante,Temperatura) VALUES ('$humedad1','$humedad2','$humedad3','$humedad4','$humedad5','$humedad6','$humedad7','$potencia_ina','$corriente_ina','$corriente_acs','$agua','$fertilizante','$temperatura')";
	    $sql2 = "INSERT INTO Datos_electricidad (PotenciaINA,CorrienteINA,CorrienteACS) VALUES ($potencia_ina','$corriente_ina','$corriente_acs')";
    }
//	echo $humedad;
////ESTE METODO TAMBIEN FUNCIONA
//	if($conn->query($sql) === TRUE){
	    //echo "Nuevo registro creado";

//	}else{
	    //echo "Error";
//	}
    if (mysqli_query($conn, $sql)) {
      echo "New record created successfully";
    } else {
//      echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
    if(mysqli_query($conn, $sql2)){
        echo "New record";
    } else{
        
    }
    
}else{
	echo "No se han envieado nada";
}
$conn ->close();
?>
