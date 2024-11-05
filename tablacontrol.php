<?php
// Verificar si hay una solicitud POST con el parámetro 'control_id' (podrías cambiar 'control_id' si no es necesario)
    header('Cache-Control: no-cache, must-revalidate');
    // Detalles de la base de datos
    $dbHost     = "127.0.0.1";
    $dbUsername = "u975775819_Kevin";       // Cambiar por tu usuario de la base de datos
    $dbPassword = "Tomates2020!";      // Cambiar por tu contraseña
    $dbName     = "u975775819_HuertoTomates";  // Cambiar por el nombre de tu base de datos
    
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
    $query = $db->query("SELECT Humedad_min, Humedad_max, Horario_enc, Horario_apg, Fertilizado_periodo, Fertilizado_reinicio, Fertilizado_recarga, Lectura_forzada, Luz FROM Tabla_Control LIMIT 1");
    
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
        $data['Luz'] = boolval($controlData['Luz']);

    } else {
        // Si no hay datos, devolver error
        $data['status'] = 'err';
        $data['result'] = '';

    }
  

    header('Content-Type: application/json charset=utf-8');
    echo json_encode($data);
    
    $sql_update = "UPDATE Tabla_Control SET Fertilizado_reinicio = 0, Fertilizado_recarga = 0, Lectura_forzada = 0 WHERE idKey = 1"; // Cambia "id" por la columna identificadora correcta
    $db->query($sql_update);
    
    $db->close();

?>
