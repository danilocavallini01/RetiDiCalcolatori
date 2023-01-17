public class Stanza {
    private String nome;
    private String stato;
    private String [] utenti;

    public Stanza(String nome, String stato, String[] utenti) {
        this.nome = nome;
        this.stato = stato;
        this.utenti = utenti;
    }

    public String getNome() {
        return nome;
    }

    public String getStato() {
        return stato;
    }

    public String[] getUtenti() {
        return utenti;
    }

    public void setStato(String stato) {
        this.stato = stato;
    }
}  