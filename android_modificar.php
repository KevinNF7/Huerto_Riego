<?php

    
    header('Cache-Control: no-cache, must-revalidate');
    // Detalles de la base de datos
    $servername = "127.0.0.1";
    $username = "Usuario";
    $password = "Contraseña";
    $dbname = "Nombre";
    
    
    $conn = new mysqli($servername,$username,$password,$dbname);
    
    // Verificar si hubo un error en la conexión
    if ($conn->connect_error) {
        die("Unable to connect database: " . $conn->connect_error);
        
    }else{}
   
   if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $humedadMinima = $_POST['Humedad_minima'];
    $humedadMaxima = $_POST['Humedad_maxima'];
    $horaEncendido = $_POST['Hora_encendido'];
    $horaApagado = $_POST['Hora_apagado'];
    $fertilizadoPeriodo = $_POST['Fertilizado_periodo'];
    $fertilizadoRecarga = $_POST['Fertilizado_recarga'];
    $fertilizadoReinicio = $_POST['Fertilizado_reinicio'];
    $lecturaForzada = $_POST['Lectura_forzada'];
    $luz = $_POST['Luz'];

    //Cuando se quiere actualizar los datos mostrados
    if($humedadMinima == 0 && $humedadMaxima == 0){
        $sql = "UPDATE Tabla_Control SET Lectura_forzada = $lecturaForzada WHERE idKey = 1";
        if ($conn->query($sql) === TRUE) {
            //echo "Datos insertados correctamente";
        } else {
            //echo "Error"
        }   
    }else{
        $sql = "UPDATE Tabla_Control SET Humedad_min = $humedadMinima, Humedad_Max = $humedadMaxima, Horario_enc = '$horaEncendido', Horario_apg = '$horaApagado', Fertilizado_periodo = $fertilizadoPeriodo, Fertilizado_recarga = $fertilizadoRecarga, Fertilizado_reinicio = $fertilizadoReinicio, Luz = $luz WHERE idKey = 1";
        if ($conn->query($sql) === TRUE) {
            //echo "Datos insertados correctamente";
        } else {
            //echo "Error"
        }    
    }
    
   }
   $conn->close();

?>
    
   
