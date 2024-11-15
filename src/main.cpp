#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QProcess>
#include <QFile>
#include <curl/curl.h>
#include <iostream>
#include <fstream>

class ScreenRecorderApp : public QWidget {
    Q_OBJECT

public:
    ScreenRecorderApp(QWidget* parent = nullptr) : QWidget(parent) {
        // Set up UI
        QVBoxLayout* layout = new QVBoxLayout(this);
        QPushButton* recordButton = new QPushButton("Start Recording", this);
        responseLabel = new QLabel("Response will appear here", this);

        layout->addWidget(recordButton);
        layout->addWidget(responseLabel);

        connect(recordButton, &QPushButton::clicked, this, &ScreenRecorderApp::startScreenRecording);
    }

private slots:
    void startScreenRecording() {
        QString filePath = "output.mp4";

        // Start FFmpeg to record the screen for 10 seconds
        QProcess ffmpegProcess;
        ffmpegProcess.start("ffmpeg", QStringList() << "-f" << "gdigrab" << "-framerate" << "30"
            << "-i" << "desktop" << "-t" << "10"
            << filePath);
        ffmpegProcess.waitForFinished();

        // Send file to Google API after recording
        if (QFile::exists(filePath)) {
            sendToGoogleAPI(filePath);
        }
        else {
            responseLabel->setText("Recording failed.");
        }
    }

    void sendToGoogleAPI(const QString& filePath) {
        // Initialize libcurl
        CURL* curl;
        CURLcode res;
        struct curl_slist* headers = NULL;

        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        if (curl) {
            headers = curl_slist_append(headers, "Authorization: Bearer YOUR_ACCESS_TOKEN");
            headers = curl_slist_append(headers, "Content-Type: application/json");

            std::ifstream videoFile(filePath.toStdString(), std::ios::binary);
            std::string fileData((std::istreambuf_iterator<char>(videoFile)),
                std::istreambuf_iterator<char>());

            curl_easy_setopt(curl, CURLOPT_URL, "https://vision.googleapis.com/v1/images:annotate");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fileData.c_str());

            // Capture the response
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                responseLabel->setText("Failed to send request to Google API.");
            }
            else {
                responseLabel->setText("Response: " + QString::fromStdString(responseBuffer));
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();
    }

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

private:
    QLabel* responseLabel;
    std::string responseBuffer;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ScreenRecorderApp window;
    window.setWindowTitle("Screen Recorder with Google API");
    window.resize(400, 200);
    window.show();

    return app.exec();
}

#include "main.moc"
