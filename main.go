package main

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"

	"github.com/prometheus/client_golang/prometheus"
	"github.com/prometheus/client_golang/prometheus/promhttp"
)

type Weather struct{
        Temperature float32 `json:"temperature_c"`
        Humidity float32 `json:"humidity_p"`
}


var ( 
        temperature = prometheus.NewGauge(
                prometheus.GaugeOpts {
                        Name: "temperature_in_celsius",
                        Help: "Current temperature in celsius.",
                },
        )
        humidity = prometheus.NewGauge(
                prometheus.GaugeOpts { 
                        Name: "humidity_percentage",
                        Help: "Percentage of atmospheric humidity.",
                },
        )
)

func handle_temp_requests(w http.ResponseWriter, r *http.Request) {
        var temp Weather
        dec := json.NewDecoder(r.Body)
        err := dec.Decode(&temp)
        if err != nil {
                http.Error(w, http.StatusText(http.StatusInternalServerError), http.StatusInternalServerError)
                fmt.Printf("could not decode message %v", err)
                return
        }
        temperature.Set(float64(temp.Temperature))
        humidity.Set(float64(temp.Humidity))
        
        fmt.Printf("%+v\n", temp)
        w.WriteHeader(http.StatusOK)
}

func handle_prom_health_check(w http.ResponseWriter, r *http.Request) {
        w.WriteHeader(http.StatusOK)
        io.WriteString(w, "OK")
}

func main() {

        prometheus.MustRegister(temperature)
        prometheus.MustRegister(humidity)
        
        http.HandleFunc("/temp", handle_temp_requests)
        http.HandleFunc("/healthcheck", handle_prom_health_check)
        http.Handle("/metrics", promhttp.Handler())
        http.ListenAndServe("0.0.0.0:8080", nil)
}
