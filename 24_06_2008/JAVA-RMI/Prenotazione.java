import java.io.Serializable;

public class Prenotazione implements Serializable{
    private String id;
    private String numPersone;
    private String tipoPrenotazione;
    private String tipoVeicolo;
    private String targa;
    private String immagine;

    public Prenotazione(String id, String numPersone, String tipoPrenotazione, String tipoVeicolo, String targa, String immagine) {
        this.id = id;
        this.numPersone = numPersone;
        this.tipoPrenotazione = tipoPrenotazione;
        this.tipoVeicolo = tipoVeicolo;
        this.targa = targa;
        this.immagine = immagine;
    }

    public String getTipoPrenotazione() {
        return tipoPrenotazione;
    }
    public String getId() {
        return id;
    }
    public String getNumPersone() {
        return numPersone;
    }
    public String getTipoVeicolo() {
        return tipoVeicolo;
    }
    public String getTarga() {
        return targa;
    }
    public String getImmagine() {
        return immagine;
    }
}
