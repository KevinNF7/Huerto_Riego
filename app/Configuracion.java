package com.example.huerto7;

import android.app.TimePickerDialog;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TimePicker;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class Configuracion extends AppCompatActivity {

    EditText et_humedadMinima, et_humedadMaxima, et_horaEncendido, et_horaApagado, et_periodo;
    Button btn_modificar, btn_reinicio, btn_recarga, btn_luz, btn_cerrar;
    String color_off, color_on;
    Retrofit retrofit;
    int humedadMin, humedadMax, fertilizadoPeriodo, fertilizadoReinicio, fertilizadoRecarga, lecturaForzada, luz;
    String horarioEnc, horarioApg;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.configuracion);

        retrofit = new Retrofit.Builder()
                .baseUrl("https://historia-del-tiempo.com/")
                .addConverterFactory(GsonConverterFactory.create())
                .build();
        fertilizadoReinicio = 0;
        fertilizadoRecarga = 0;
        obtenerDatosConfiguracion();
        color_on = "#ffe033";
        color_off = "#FF4D565F";
        et_humedadMinima = findViewById(R.id.etHumedadMinima);
        et_humedadMaxima = findViewById(R.id.etHumedadMaxima);
        et_horaEncendido = findViewById(R.id.etHoraEncendido);
        et_horaApagado = findViewById(R.id.etHoraApagado);
        et_periodo = findViewById(R.id.etPeriodo);
        btn_modificar = findViewById(R.id.btnModificar);
        btn_reinicio = findViewById(R.id.btnReinicio);
        btn_recarga = findViewById(R.id.btnRecarga);
        btn_luz = findViewById(R.id.btnLuz);
        btn_cerrar = findViewById(R.id.btnCerrar);


               
        et_horaEncendido.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Calendar calendar = Calendar.getInstance();
                int hora = calendar.get(Calendar.HOUR_OF_DAY);
                int minuto = calendar.get(Calendar.MINUTE);

                TimePickerDialog timePickerDialog = new TimePickerDialog(Configuracion.this, new TimePickerDialog.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(TimePicker timePicker, int horaD, int minutoD) {
                         et_horaEncendido.setText(String.format(Locale.getDefault(),"%02d:%02d",horaD,minutoD));
                    }
                },hora,minuto,true);
                timePickerDialog.show();
            }
        });
        et_horaApagado.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Calendar calendar = Calendar.getInstance();
                int hora = calendar.get(Calendar.HOUR_OF_DAY);
                int minuto = calendar.get(Calendar.MINUTE);

                TimePickerDialog timePickerDialog = new TimePickerDialog(Configuracion.this, new TimePickerDialog.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(TimePicker timePicker, int horaD, int minutoD) {
                        et_horaEncendido.setText(String.format(Locale.getDefault(),"%02d:%02d",horaD,minutoD));
                    }
                },hora,minuto,true);
                timePickerDialog.show();
            }
        });
        btn_cerrar.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                finish();
            }
        });
        btn_modificar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                enviarDatos();
            }
        });
        btn_reinicio.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                fertilizadoReinicio = 1;
                enviarDatos();
            }
        });
        btn_recarga.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                fertilizadoRecarga = 1;
                enviarDatos();

            }
        });
        btn_luz.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(!isHoraDeEncendido(horarioEnc,horarioApg)) {
                    if (luz == 0) {
                        luz = 1;
                        btn_luz.setBackgroundColor(Color.parseColor(color_on));
                        enviarDatos();
                    } else {
                        luz = 0;
                        btn_luz.setBackgroundColor(Color.parseColor(color_off));
                        enviarDatos();
                    }
                }
            }
        });
    }
    private void obtenerDatosConfiguracion(){
        RetrofitPeticion peticion = retrofit.create(RetrofitPeticion.class);

        Call<RetrofitControl> call = peticion.getDatosControl();
        call.enqueue(new Callback<RetrofitControl>() {
            @Override
            public void onResponse(Call<RetrofitControl> call, Response<RetrofitControl> response) {
                if(response.isSuccessful() && response.body() != null){
                    RetrofitControl datos = response.body();

                    et_humedadMinima.setText(String.valueOf((datos.getHumedadMin()-1023)*100/(-1023)));
                    et_humedadMaxima.setText(String.valueOf((datos.getHumedadMax()-1023)*100/(-1023)));
                    et_horaEncendido.setText(datos.getHorarioEnc());
                    et_horaApagado.setText(datos.getHorarioApg());
                    et_periodo.setText(String.valueOf(datos.getFertilizadoPeriodo()));
                    horarioApg = datos.getHorarioApg();
                    horarioEnc = datos.getHorarioEnc();



                    if(datos.getLuz()==0){
                        btn_luz.setBackgroundColor(Color.parseColor(color_off));
                        luz = 0;
                    }
                    else{
                        btn_luz.setBackgroundColor(Color.parseColor(color_on));
                        luz = 1;
                    }
                    fertilizadoReinicio = datos.getFertilizadoReinicio();
                    fertilizadoRecarga = datos.getFertilizadoRecarga();
                }else{
                }
            }
            @Override
            public void onFailure(Call<RetrofitControl> call, Throwable t) {
                //tv_humedad1.setText("Failure");
            }
        });
    }
    public void enviarDatos(){
        humedadMin = (int) (Integer.parseInt(et_humedadMinima.getText().toString()) * -10.23 + 1023);
        humedadMax = (int)(Integer.parseInt(et_humedadMaxima.getText().toString()) * -10.23 + 1023);
        horarioEnc = et_horaEncendido.getText().toString();
        horarioApg = et_horaApagado.getText().toString();
        fertilizadoPeriodo = Integer.parseInt(et_periodo.getText().toString());

        RetrofitPeticion peticion = retrofit.create(RetrofitPeticion.class);

        Call<ResponseBody> callEnvio = peticion.enviarDatos(humedadMin,humedadMax,horarioEnc,horarioApg,fertilizadoPeriodo,fertilizadoRecarga,fertilizadoReinicio,0,luz);
        callEnvio.enqueue(new Callback<ResponseBody>() {
            @Override
            public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {
                Toast.makeText(getApplicationContext(), "Actualizado", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onFailure(Call<ResponseBody> call, Throwable t) {
                Toast.makeText(getApplicationContext(), "Error", Toast.LENGTH_SHORT).show();
            }
        });
        fertilizadoRecarga = 0;
        fertilizadoReinicio = 0;
    }
    public boolean isHoraDeEncendido(String horaEncendido, String horaApagado) {
        try {
            // Formato de la hora (HH:mm)
            SimpleDateFormat sdf = new SimpleDateFormat("HH:mm", Locale.getDefault());

            // Hora actual
            Calendar currentCalendar = Calendar.getInstance();
            Date currentDate = currentCalendar.getTime();
            String currentTimeStr = sdf.format(currentDate);
            Date currentTime = sdf.parse(currentTimeStr);

            
            Date encendidoTime = sdf.parse(horaEncendido);
            Date apagadoTime = sdf.parse(horaApagado);

            // Crear Calendar para las horas de encendido y apagado
            Calendar encendidoCalendar = Calendar.getInstance();
            encendidoCalendar.setTime(encendidoTime);

            Calendar apagadoCalendar = Calendar.getInstance();
            apagadoCalendar.setTime(apagadoTime);

            // Comparar si la hora actual está dentro del rango
            if (currentTime.after(encendidoTime) && currentTime.before(apagadoTime)) {
                return true;
            } else {
                return false;
            }

        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }
}
