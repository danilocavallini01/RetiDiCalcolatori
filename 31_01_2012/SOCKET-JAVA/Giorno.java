public class Giorno {
    private String giorno;
    private String previsione;
    private int tMin;
    private int tMax;
    private String audio;

    public String getGiorno() {
        return giorno;
    }

    public String getPrevisione() {
        return previsione;
    }

    public int gettMin() {
        return tMin;
    }

    public int gettMax() {
        return tMax;
    }

    public String getAudio() {
        return audio;
    }

    public void setAudio(String audio) {
        this.audio = audio;
    }

    public Giorno(String giorno, String previsione, int tMin, int tMax, String audio) {
        this.giorno = giorno;
        this.previsione = previsione;
        this.tMin = tMin;
        this.tMax = tMax;
        this.audio = audio;
    }
}