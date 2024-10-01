<?php
    //Codigo para evitar almacenamiento en memoria cache
    header('Cache-Control: no-cache, must-revalidate');
    // Detalles de la base de datos
    $dbHost     = "127.0.0.1";
    $dbUsername = "Usuario";       
    $dbPassword = "Contraseña";      
    $dbName     = "Base_de_datos";  /
    
    $data = array();
    // Crear la conexión y seleccionar la base de datos
    $db = new mysqli($dbHost, $dbUsername, $dbPassword, $dbName);
    
    // Verificar si hubo un error en la conexión
    if ($db->connect_error) {
        echo "Coneccion fin";
        die("Unable to connect database: " . $db->connect_error);
        
    }else{
        
    }
   
    // Realizar la consulta para obtener los datos de 'Tabla_Control'
    $query = $db->query("SELECT Humedad_min, Humedad_max, Horario_enc, Horario_apg, Fertilizado_periodo, Fertilizado_reinicio, Fertilizado_recarga, Lectura_forzada FROM Tabla_Control LIMIT 1");
    
    // Verificar si se encontró al menos una fila
    if ($query->num_rows > 0) {
        $controlData = $query->fetch_assoc();
        
        // Guardar los datos obtenidos en un array
        
        $data['Humedad_min'] = intval($controlData['Humedad_min']);
        $data['Humedad_max'] = intval($controlData['Humedad_max']);
        $data['Horario_enc'] = $controlData['Horario_enc'];
        $data['Horario_apg'] = $controlData['Horario_apg'];
        $data['Fertilizado_periodo'] = intval($controlData['Fertilizado_periodo']);
        $data['Fertilizado_reinicio'] = boolval($controlData['Fertilizado_reinicio']);
        $data['Fertilizado_recarga'] = boolval($controlData['Fertilizado_recarga']);
        $data['Lectura_forzada'] = boolval($controlData['Lectura_forzada']);
        
       

    } else {
        

    }

    header('Content-Type: application/json charset=utf-8');
    echo json_encode($data);
    $db->close();

?>
