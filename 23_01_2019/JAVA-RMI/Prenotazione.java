
public class Prenotazione {
    private String targa;
    private String patente;
    private String tipo;
    private String immagini;

    public Prenotazione() {
        this.targa = "L";
        this.patente = "L";
        this.tipo = "L";
        this.immagini = "L";
    }

    public Prenotazione(String targa, String patente, String tipo, String immagini) {
        this.targa = targa;
        this.patente = patente;
        this.tipo = tipo;
        this.immagini = immagini;
    }

    public String getTarga() {
        return targa;
    }

    public String getPatente() {
        return patente;
    }

    public String getTipo() {
        return tipo;
    }

    public String getImmagini() {
        return immagini;
    }
}
