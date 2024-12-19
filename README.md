# TP_TFTP_Henner_Lukas

Pénélope HENNER: penelope.henner@ensea.fr  
Maël LUKAS: mael.lukas@ensea.fr

# Lab Work Synthesis - TFTP Client 

**Objectives: Develop a TFTP client using RFC sepcifications and Wireshark captures**

We have created a client program capable of exchanging files using the Trivial File Transfer
Protocol (TFTP). TFTP is commonly used for network installation of operating systems (PXE protocol), disk-
less node operations, and firmware updates for network equipment (routers, IoT, etc.).

The purpose of the lab is to create two clients to be used from the commande line:

- One for downloading a file from the server: 
		$ gettftp host file

- One for uploading a file to the server: 
		$ puttftp host file


## Tree

```
.
├── README.md
├── gettftp.c
└── puttftp.c
```

