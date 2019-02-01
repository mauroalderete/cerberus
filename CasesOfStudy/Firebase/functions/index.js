'use strict';

const functions = require('firebase-functions');
const nodemailer = require('nodemailer');
// Configure the email transport using the default SMTP transport and a GMail account.
// For other types of transports such as Sendgrid see https://nodemailer.com/transports/
// TODO: Configure the `gmail.email` and `gmail.password` Google Cloud environment variables.
//const gmailEmail = functions.config().gmail.email;
//const gmailPassword = functions.config().gmail.password;
const mailTransport = nodemailer.createTransport({
  service: 'gmail',
  auth: {
    user: "rayquen.zero@gmail.com",
    pass: "C4rlS4g4n",
  },
});

// Sends an email confirmation when a user changes his mailing list subscription.
exports.sendEmailConfirmation = functions.database.ref('/system').onWrite((change) => {

  const mailOptions = {
    from: '"Rayquen DEV" <noreply@firebase.com>',
    to: "alderete.mauro@gmail.com",
  };

  mailOptions.subject = "Prueba Firebase Function 001";
  mailOptions.text = "Esta es una prueba de las capacidades de mensajeria de functions Firebase";

  return mailTransport.sendMail(mailOptions)
    .then(() => console.log(`Mensaje enviado`))
    .catch((error) => console.error('There was an error while sending the email:', error));
});