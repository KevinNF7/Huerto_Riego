<?php

    
    header('Cache-Control: no-cache, must-revalidate');
    // Detalles de la base de datos
    date_default_timezone_set('America/Lima');
    $servername = "127.0.0.1";
    $username = "Usuario";
    $password = "Contraseña";
    $dbname = "u975775819_HuertoTomates";
    
    $data = array();
    // Crear la conexión y seleccionar la base de datos
    $conn = new mysqli($servername,$username,$password,$dbname);
    
    // Verificar si hubo un error en la conexión
    if ($conn->connect_error) {
        die("Unable to connect database: " . $conn->connect_error);
        
    }else{}
   

    // Realizar la consulta para obtener los datos de 'Tabla_Control'
    $query = $conn->query("SELECT Humedad_min, Humedad_max, DATE_FORMAT(Horario_enc, '%H:%i') AS Horario_enc, 
       DATE_FORMAT(Horario_apg, '%H:%i') AS Horario_apg, Fertilizado_periodo, Fertilizado_reinicio, Fertilizado_recarga, Lectura_forzada, Luz FROM Tabla_Control WHERE idKey = 1");
    
    // Verificar si se encontró al menos una fila
    if ($query->num_rows > 0) {
        $controlData = $query->fetch_assoc();
        
        // Guardar los datos obtenidos en un array
        
        $data['Humedad_min'] = intval($controlData['Humedad_min']);
        $data['Humedad_max'] = intval($controlData['Humedad_max']);
        $data['Horario_enc'] = $controlData['Horario_enc'];
        $data['Horario_apg'] = $controlData['Horario_apg'];
        $data['Fertilizado_periodo'] = intval($controlData['Fertilizado_periodo']);
        $data['Fertilizado_reinicio'] = intval($controlData['Fertilizado_reinicio']);
        $data['Fertilizado_recarga'] = intval($controlData['Fertilizado_recarga']);
        $data['Lectura_forzada'] = intval($controlData['Lectura_forzada']);
        $data['Luz'] = intval($controlData['Luz']);


    } else {}
  

    header('Content-Type: application/json charset=utf-8');
    echo json_encode($data);

    $conn->close();

?>
