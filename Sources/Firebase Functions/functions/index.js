'use strict';

const functions = require('firebase-functions');
const nodemailer = require('nodemailer');
// TODO: Configure the `gmail.email` and `gmail.password` Google Cloud
// environment variables.
const gmailEmail = "rayquen.zero@gmail.com"
const gmailPassword = "C4rlS4g4n"
const mailTransport = nodemailer.createTransport({
    service: 'gmail',
    auth: {
        user: gmailEmail,
        pass: gmailPassword
    }
});
const reciverTest = "alderete.mauro@gmail.com"
const reciversRun = "alderete.mauro@gmail.com, andres.sergio95@gmail.com, galarza.arcelia@gmail.com, alderete_jose@yahoo.com.ar"
var recivers = reciverTest

exports.sendEmailSensorActivated = functions
    .database
    .ref('/sensors/{id}/status')
    .onUpdate((change, context) => {
        const snapshot = change.after;
        const val = snapshot.val();
        const id = context.params.id;

        console.log("snapshot...");
        console.log(snapshot);
        console.log("val: " + val);

        if (val != 'ON_ACTIVATED') {
            return null;
        } else {
            const mailOptions = {
                from: '"Cerberus" <noreply@firebase.com>',
                to: recivers
            };

            mailOptions.subject = "ALERTA Movimiento detectado";
            mailOptions.text = "Se ha detectado movimiento en el sensor " + id;

            return mailTransport
                .sendMail(mailOptions)
                .then(() => console.log('ALERT enviada'))
                .catch(
                    (error) => console.error('There was an error while sending the email:', error)
                );
        }
    });

exports.sendEmailPowerChange = functions
    .database
    .ref('/power/status')
    .onUpdate((change) => {
        const snapshotBefore = change.before;
        const snapshotAfter = change.after;
        const valBefore = snapshotBefore.val();
        const valAfter = snapshotAfter.val();

        let statusRecognition = false;

        const mailOptions = {
            from: '"Cerberus" <noreply@firebase.com>',
            to: recivers
        };

        if (valBefore == 'AVAILABLE_CONNECTED' && valAfter == 'UNAVAILABLE') {
            statusRecognition = true;

            let msg = "Se detecto un corte de luz. Se esta utilizando las baterias.\n\r";;
            msg += "Por precaución se desactivo el uso de la red electrica temporariamente.\n\r";
            msg += "Esto significa que si la electricidad regresa, Cerberus no se conectara a la red " +
                    "hasta que se lo indique.";

            mailOptions.subject = "ADVERTENCIA Corte de luz";
            mailOptions.text = msg;

            return mailTransport
                .sendMail(mailOptions)
                .then(() => console.log('SHUTDOWN enviada'))
                .catch(
                    (error) => console.error('There was an error while sending the email:', error)
                );
        }

        if (valBefore == 'UNAVAILABLE' && valAfter == 'AVAILABLE_DISCONNECTED') {
            statusRecognition = true;

            let msg = "La electricidad volvio a la red.\n\r";
            msg += "Cerberus puede volver a conectarse a la red electrica si se lo indica desde la aplicacion," +
                    "pero se recomienda esperar unos minutos por precuación.";

            mailOptions.subject = "AVISO La luz regreso";
            mailOptions.text = msg;

            return mailTransport
                .sendMail(mailOptions)
                .then(() => console.log('LIGHT BACK enviado'))
                .catch(
                    (error) => console.error('There was an error while sending the email:', error)
                );
        }

        if (statusRecognition == false) {
            return null;
        }
    });

exports.sendEmailLifeSignal = functions
    .database
    .ref('/system/lifeSignal')
    .onUpdate((change) => {
        const mailOptions = {
            from: '"Cerberus" <noreply@firebase.com>',
            to: recivers
        };
        let msg = "Cerberus sigue conectado y muestra señal de vida\n\r";
        msg += "Desde la ultima vez que Cerberus mostro señal de vida, ocurrieron los siguient" +
                "es eventos:\n\r";
        msg += "Reinicios: ?\n\r";
        msg += "Reconecciones WIFI: ?\n\r";
        msg += "Reconecciones Base de datos: ?\n\r";
        msg += "Alertas activadas: ?\n\r";
        msg += "Cortes de luz: ?\n\r";
        mailOptions.subject = "Señal de vida";
        mailOptions.text = msg;

        return mailTransport
            .sendMail(mailOptions)
            .then(() => console.log('LIFE SIGNAL enviado'))
            .catch(
                (error) => console.error('There was an error while sending the email:', error)
            );

    });

   
exports.sendEmailNewReset = functions
    .database
    .ref('/system/newReset')
    .onUpdate((change) => {
        const mailOptions = {
            from: '"Cerberus" <noreply@firebase.com>',
            to: recivers
        };
        let msg = "Cerberus acaba de reiniciarse. Se desconocen la causa.\n\r";
        msg += "Se recomienda realizar una inspección."
        mailOptions.subject = "ADVERTENCIA Cerberus se reinicio";
        mailOptions.text = msg;

        return mailTransport
            .sendMail(mailOptions)
            .then(() => console.log('NEW RESET enviado'))
            .catch(
                (error) => console.error('There was an error while sending the email:', error)
            );

    });
    