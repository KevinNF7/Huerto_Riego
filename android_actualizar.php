<?php

    
    header('Cache-Control: no-cache, must-revalidate');
    // Detalles de la base de datos
    date_default_timezone_set('America/Lima');
    $servername = "127.0.0.1";
    $username = "u975775819_Kevin";
    $password = "Tomates2020!";
    $dbname = "u975775819_HuertoTomates";
    
    $data = array();
    // Crear la conexión y seleccionar la base de datos
    $conn = new mysqli($servername,$username,$password,$dbname);
    
    // Verificar si hubo un error en la conexión
    if ($conn->connect_error) {
        die("Unable to connect database: " . $conn->connect_error);
        
    }else{}
   
   $fecha = date("Ymd");

    // Nombre de la tabla basada en la fecha
    $nombre_tabla = "Datos_" . $fecha;
    
   
    // Realizar la consulta para obtener los datos de 'Tabla_Control'
    $query = $conn->query("SELECT Humedad1, Humedad2, Humedad3, Humedad4, Humedad5, Humedad6, Humedad7, NivelDeAgua, Temperatura FROM Datos_20240910 ORDER BY idKey DESC LIMIT 1");
    
    // Verificar si se encontró al menos una fila
    if ($query->num_rows > 0) {
        $controlData = $query->fetch_assoc();
        
        // Guardar los datos obtenidos en un array
        
        $data['Humedad_1'] = intval($controlData['Humedad1']);
        $data['Humedad_2'] = intval($controlData['Humedad2']);
        $data['Humedad_3'] = intval($controlData['Humedad3']);
        $data['Humedad_4'] = intval($controlData['Humedad4']);
        $data['Humedad_5'] = intval($controlData['Humedad5']);
        $data['Humedad_6'] = intval($controlData['Humedad6']);
        $data['Humedad_7'] = intval($controlData['Humedad7']);
        $data['Nivel_de_agua'] = intval($controlData['NivelDeAgua']);
        $data['Temperatura'] = intval($controlData['Temperatura']);


    } else {}
  

    header('Content-Type: application/json charset=utf-8');
    echo json_encode($data);

    $conn->close();

?>
