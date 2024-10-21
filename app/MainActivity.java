package com.example.huerto7;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;

import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class MainActivity extends AppCompatActivity {

    TextView tv_humedad1, tv_humedad2, tv_humedad3, tv_humedad4, tv_humedad5 , tv_humedad6, tv_humedad7;
    TextView tv_nivel_agua, tv_temperatura, tv_humedad_promedio;
    ProgressBar progressBar;
    Button btn_actualizar, btn_configuracion;
    Retrofit retrofit;

    int humedadMin, humedadMax, fertilizadoPeriodo, fertilizadoReinicio, fertilizadoRecarga, lecturaForzada, luz;
    String horarioEnc, horarioApg;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);
        //ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
        //    Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
        //    v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
        //    return insets;
        //});
        //Button btnConfig = findViewById(R.id.btn_configuracion);
       // btnConfig.setOnClickListener(v -> {
       //     showConfigDialog();
       // });

        retrofit = new Retrofit.Builder()
                .baseUrl("https://historia-del-tiempo.com/")
                .addConverterFactory(GsonConverterFactory.create())
                .build();
        tv_humedad1 = findViewById(R.id.tw_humedad_1_valor);
        tv_humedad2 = findViewById(R.id.tw_humedad_2_valor);
        tv_humedad3 = findViewById(R.id.tw_humedad_3_valor);
        tv_humedad4 = findViewById(R.id.tw_humedad_4_valor);
        tv_humedad5 = findViewById(R.id.tw_humedad_5_valor);
        tv_humedad6 = findViewById(R.id.tw_humedad_6_valor);
        tv_humedad7 = findViewById(R.id.tw_humedad_7_valor);
        tv_humedad_promedio = findViewById(R.id.tw_humedad_promedio_valor);
        tv_nivel_agua = findViewById(R.id.tw_agua_porcentaje);
        progressBar = findViewById(R.id.progress_bar_agua);
        tv_temperatura = findViewById(R.id.tw_temperatura_valor);
        btn_actualizar = findViewById(R.id.btn_actualizar);
        btn_configuracion = findViewById(R.id.btn_configuracion);

        btn_actualizar.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){

                obtenerDatos();

                new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        actualizarDatos();;
                    }
                }, 20000);
            }
        });
        btn_configuracion.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this,Configuracion.class);
                startActivity(intent);
            }
        });
        actualizarDatos();
    }

    private void actualizarDatos() {

        RetrofitPeticion peticion = retrofit.create(RetrofitPeticion.class);

        Call<RetrofitDatos> call = peticion.getDatos();
        call.enqueue(new Callback<RetrofitDatos>() {
            @SuppressLint("SetTextI18n")
            @Override
            public void onResponse(Call<RetrofitDatos> call, Response<RetrofitDatos> response) {
                if(response.isSuccessful() && response.body() != null){
                    RetrofitDatos datos = response.body();

                    tv_humedad1.setText(String.valueOf((datos.getHumedad1()-1023)*100/(-1023))+"%");
                    tv_humedad2.setText(String.valueOf((datos.getHumedad2()-1023)*100/(-1023))+"%");
                    tv_humedad3.setText(String.valueOf((datos.getHumedad3()-1023)*100/(-1023))+"%");
                    tv_humedad4.setText(String.valueOf((datos.getHumedad4()-1023)*100/(-1023))+"%");
                    tv_humedad5.setText(String.valueOf((datos.getHumedad5()-1023)*100/(-1023))+"%");
                    tv_humedad6.setText(String.valueOf((datos.getHumedad6()-1023)*100/(-1023))+"%");
                    tv_humedad7.setText(String.valueOf((datos.getHumedad7()-1023)*100/(-1023))+"%");

                    int promedio = (datos.getHumedad1() + datos.getHumedad2() + datos.getHumedad3() +
                            datos.getHumedad4() + datos.getHumedad5() + datos.getHumedad6() + datos.getHumedad7())/7;
                    tv_humedad_promedio.setText(String.valueOf((promedio-1023)*100/(-1023))+"%");
                    tv_temperatura.setText(String.valueOf(datos.getTemperatura()));
                    double valor_agua = datos.getAgua();
                    double altura_agua = 55 - (valor_agua*0.034)/2;
                    double radio_agua = 18 + ((22.5-18)*altura_agua)/55;
                    double volumen_agua = (0.3333 * Math.PI * altura_agua * (Math.pow(radio_agua,2) + radio_agua * 18 + Math.pow(18,2)))/1000;
                    double porcentaje = (volumen_agua/70)*100;
                    tv_nivel_agua.setText( String.format("%.1f",porcentaje) + "%");
                    progressBar.setProgress((int) porcentaje);

                }else{
                }
            }
            @Override
            public void onFailure(Call<RetrofitDatos> call, Throwable t) {
                //tv_humedad1.setText("Failure");
            }
        });
    }
    private void obtenerDatos(){
        RetrofitPeticion peticion = retrofit.create(RetrofitPeticion.class);

        Call<ResponseBody> callEnvio = peticion.enviarDatos(0,0,"1","1",0,0,0,1,0);
        callEnvio.enqueue(new Callback<ResponseBody>() {
            @Override
            public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {

            }

            @Override
            public void onFailure(Call<ResponseBody> call, Throwable t) {

            }
        });
    }

}
