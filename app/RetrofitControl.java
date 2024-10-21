package com.example.huerto7;
import com.google.gson.annotations.SerializedName;
public class RetrofitControl {
    @SerializedName("Humedad_min")
    private int humedadMin;

    @SerializedName("Humedad_max")
    private int humedadMax;

    @SerializedName("Horario_enc")
    private String horarioEnc;

    @SerializedName("Horario_apg")
    private String horarioApg;

    @SerializedName("Fertilizado_periodo")
    private int fertilizadoPeriodo;

    @SerializedName("Fertilizado_reinicio")
    private int fertilizadoReinicio;

    @SerializedName("Fertilizado_recarga")
    private int fertilizadoRecarga;

    @SerializedName("Lectura_forzada")
    private int lecturaForzada;

    @SerializedName("Luz")
    private int luz;
    // Getters y setters
    public int getHumedadMin() {return humedadMin;}
    public int getHumedadMax() {return humedadMax;}
    public String getHorarioEnc() {return horarioEnc;}
    public String getHorarioApg() {return horarioApg;}
    public int getFertilizadoPeriodo() {return fertilizadoPeriodo;}
    public int getFertilizadoReinicio() {return fertilizadoReinicio;}
    public int getFertilizadoRecarga() {return fertilizadoRecarga;}
    public int getLecturaForzada() {return lecturaForzada;}
    public int getLuz() {return luz;}

}
