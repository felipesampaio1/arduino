class Bico{
 public:
    Bico (String nome, int solenoide, int medidorFluxo){
      pinMode(solenoide, OUTPUT);
      pinMode(medidorFluxo, INPUT);
      preco = 0;
    }

    void setPreco(float p){
      preco = p;
    }

    float getPreco(){
      return preco;
    }

    
 private:
   String nome;
   int solenoide, medidorFluxo;
   float preco;
};
