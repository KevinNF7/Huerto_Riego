package com.example.huerto7;
import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.http.Field;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.POST;
import retrofit2.http.Query;
public interface RetrofitPeticion {
    @GET("android_actualizar.php")
    Call<RetrofitDatos> getDatos();

    @GET("android_datosControl.php")
    Call<RetrofitControl> getDatosControl();

    @FormUrlEncoded
    @POST("android_modificar.php")
    Call<ResponseBody> enviarDatos(
            @Field("Humedad_minima") int humedadMinima,
            @Field("Humedad_maxima") int humedadMaxima,
            @Field("Hora_encendido") String horaEncendido,
            @Field("Hora_apagado") String horaApagado,
            @Field("Fertilizado_periodo") int fertilizadoPeriodo,
            @Field("Fertilizado_recarga") int FertilizadoRecarga,
            @Field("Fertilizado_reinicio") int fertilizadoReinicio,
            @Field("Lectura_forzada") int lecturaForzada,
            @Field("Luz") int luz


    );

}

