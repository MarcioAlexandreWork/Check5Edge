# ESPK1

# Márcio Alexandre 566238
# Fábio Apolinário 561828
# Leonardo Lopes 562171

Monitoramento de umidade, temperatura, luminosidade e distância que utiliza o IP público 9.234.138.146 para publicações MQTT na lamp007

- Não foi possível fazer a comunicação entre o wokwi e o IoT atualmente por conta da queda do IP, mas se trocar as variáveis de otimização de comunicação, é possível que funcione e as informações sejam publicadas

- Sobre os requests GET no postman foi possível apenas ser realizados ontem nas portas 4041 e 8666 para um health check, já para ver a versão do Orion (1026) não foi possível

- Sobre a criação da IOT Fiware foi possível criar uma VM no VMware utilizando como iso o Ubuntu server em sua ultima versão, mas não foi possível a criação do servidor e nem do IP usando NODE RED



Link do projeto no Wokwi: https://wokwi.com/projects/445335974803074049


Request GET da porta 8666
<img width="1574" height="727" alt="image" src="https://github.com/user-attachments/assets/25f1b80d-eae8-4cc3-9298-24895243455d" />

Request GET da porta 4041
<img width="1597" height="808" alt="image" src="https://github.com/user-attachments/assets/474e2b52-1a8b-49b2-9d3f-7de71eb98fd9" />

Tentativa de conexão ao broker falhando por conta da morte do IP público
<img width="1598" height="757" alt="image" src="https://github.com/user-attachments/assets/ea584830-2072-4b5b-b4eb-49938de00ae8" />

Tentativa da criação da configuração de uma Ubuntu Server via VMware
<img width="1197" height="283" alt="image" src="https://github.com/user-attachments/assets/c262a464-c24b-47bd-a8b5-628cb1472bd8" />
