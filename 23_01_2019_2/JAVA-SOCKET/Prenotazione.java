public class Prenotazione {
    private String targa;
    private String patente;
    private String tipoVeicolo;
    private String immagini;
    
    public Prenotazione(String targa, String patente, String tipoVeicolo, String immagini) {
        this.targa = targa;
        this.patente = patente;
        this.tipoVeicolo = tipoVeicolo;
        this.immagini = immagini;
    }

    public String getTarga() {
        return targa;
    }

    public String getPatente() {
        return patente;
    }

    public String getTipoVeicolo() {
        return tipoVeicolo;
    }

    public String getImmagini() {
        return immagini;
    }
}
