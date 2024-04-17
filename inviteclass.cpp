
#include "inviteclass.h"
#include "ui_inviteclass.h"
#include <QDebug>
#include <QMessageBox>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFileDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QPainter>
#include <QPdfWriter>

bool inviteclass::tablePopulated = false; // Initialize the static variable

inviteclass::inviteclass(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::inviteclass)
{
    ui->setupUi(this);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::activated),
               [=](int index){
                   QString columnName = ui->comboBox->itemText(index);
                   sortTable(columnName);
               });
    connect(ui->comboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &inviteclass::on_comboBox_2_currentIndexChanged);
    // Assuming your pushButton_4 object is named pushButton_4
    connect(ui->pushButton_4, &QPushButton::clicked, this, &inviteclass::extractAsPdf);

    // Check if the table is empty and not already populated before populating it
    if (ui->tableWidget->rowCount() == 0 && !tablePopulated) {
        clearTable(); // Clear the table before populating it
        populateTableFromDatabase();
        tablePopulated = true; // Set tablePopulated to true after populating the table
    }
}

void inviteclass::clearTable()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
}

void inviteclass::populateTableFromDatabase()
{
    QSqlQuery query("SELECT * FROM INVITE");
    while (query.next()) {
        QString id = query.value(0).toString();
        QString nom = query.value(1).toString();
        QString prenom = query.value(2).toString();
        QString sexe = query.value(3).toString();
        QString age = query.value(4).toString();
        QString NTA = query.value(5).toString();
        QDate DDP = query.value(6).toDate();
        QString montant = query.value(7).toString(); // Assuming "Montant" is in column index 7
        QString phone = query.value(8).toString(); // Assuming "Phone" is in column index 8

        addToTable(id, nom, prenom, sexe, age, NTA, montant, phone, DDP);
    }
}
inviteclass::~inviteclass()
{
    // Disconnect the signal-slot connection for the "Edit" button
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QPushButton *editButton = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(row, 8));
        if (editButton) {
            disconnect(editButton, &QPushButton::clicked, nullptr, nullptr);
        }
    }

    delete ui;
}

void inviteclass::on_pushButton_clicked()
{
    QString id = ui->lineEdit->text();
    QString nom = ui->lineEdit_2->text();
    QString prenom = ui->lineEdit_3->text();
    QString sexe = ui->radioButton->isChecked() ? "Homme" : "Femme";
    QString age = ui->lineEdit_5->text();
    QString NTA = ui->lineEdit_6->text();
    QString montant = ui->lineEdit_10->text();
    QString phone = ui->lineEdit_11->text();
    QDate DDP = ui->dateEdit->date();


    if (id.isEmpty()|| nom.isEmpty()  || prenom.isEmpty() || age.isEmpty() || NTA.isEmpty() || montant.isEmpty() || phone.isEmpty()) {
        QMessageBox::critical(this, "Error", "All fields are mandatory.");
        return;
    }

    addToTable(id, nom, prenom, sexe, age, NTA, montant, phone, DDP);
}

void inviteclass::addToTable(const QString &id, const QString &nom, const QString &prenom,
                             const QString &sexe, const QString &age, const QString &NTA, const QString &montant, const QString &phone,
                             const QDate &DDP)
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    // Populate the row with data
    QTableWidgetItem *item_id = new QTableWidgetItem(id);
    QTableWidgetItem *item_nom = new QTableWidgetItem(nom);
    QTableWidgetItem *item_prenom = new QTableWidgetItem(prenom);
    QTableWidgetItem *item_sexe = new QTableWidgetItem(sexe);
    QTableWidgetItem *item_age = new QTableWidgetItem(age);
    QTableWidgetItem *item_NTA = new QTableWidgetItem(NTA);
    QTableWidgetItem *item_DDP = new QTableWidgetItem(DDP.toString("yyyy-MM-dd"));
    QTableWidgetItem *item_montant = new QTableWidgetItem(montant);
    QTableWidgetItem *item_phone = new QTableWidgetItem(phone);


    ui->tableWidget->setItem(row, 0, item_id);
    ui->tableWidget->setItem(row, 1, item_nom);
    ui->tableWidget->setItem(row, 2, item_prenom);
    ui->tableWidget->setItem(row, 3, item_sexe);
    ui->tableWidget->setItem(row, 4, item_age);
    ui->tableWidget->setItem(row, 5, item_NTA);
    ui->tableWidget->setItem(row, 6, item_montant);
    ui->tableWidget->setItem(row, 7, item_phone);
    ui->tableWidget->setItem(row, 8, item_DDP);

    addEditButton(row);
    addDeleteButton(row);

    QSqlQuery query;
    query.prepare("INSERT INTO INVITE (id, nom, prenom, sexe, age, NTA, DDP,montant, phone) "
                  "VALUES (:id, :nom, :prenom, :sexe, :age, :NTA, :DDP, :montant, :phone)");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":sexe", sexe);
    query.bindValue(":age", age);
    query.bindValue(":NTA", NTA);
    query.bindValue(":montant", montant);
    query.bindValue(":phone", phone);
    query.bindValue(":DDP", DDP);

    if (query.exec()) {
        qDebug() << "Data inserted into database successfully!";
    } else {
        qDebug() << "Failed to insert data into database:" << query.lastError().text();
    }
}


void inviteclass::addEditButton(int row)
{
    QPushButton *editButton = new QPushButton("Edit");
    connect(editButton, &QPushButton::clicked, [=]() {
        on_editButton_clicked(row); // Call the slot for editing with the row index
    });

    ui->tableWidget->setCellWidget(row, 10, editButton); // Add the edit button to the table
}



void inviteclass::addDeleteButton(int row)
{
    QPushButton *deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, [=]() {
        QString id = ui->tableWidget->item(row, 0)->text();

        ui->tableWidget->removeRow(row);

        QSqlQuery query;
        query.prepare("DELETE FROM INVITE WHERE id = :id");
        query.bindValue(":id", id);
        if (query.exec()) {
            qDebug() << "Row deleted from database successfully!";
        } else {
            qDebug() << "Failed to delete row from database:" << query.lastError().text();
        }
    });

    ui->tableWidget->setCellWidget(row, 9, deleteButton);
}

void inviteclass::on_editButton_clicked(int row)
{
    QString id = ui->tableWidget->item(row, 0)->text();
    QString nom = ui->tableWidget->item(row, 1)->text();
    QString prenom = ui->tableWidget->item(row, 2)->text();
    QString sexe = ui->tableWidget->item(row, 3)->text();
    QString age = ui->tableWidget->item(row, 4)->text();
    QString NTA = ui->tableWidget->item(row, 5)->text();
    QString montant = ui->tableWidget->item(row, 6)->text();
    QString phone = ui->tableWidget->item(row, 7)->text();
    QDate DDP = QDate::fromString(ui->tableWidget->item(row, 8)->text(), "yyyy-MM-dd");

    // Pass the data to the input fields for editing
    ui->lineEdit->setText(nom);
    ui->lineEdit_2->setText(id);
    ui->lineEdit_3->setText(prenom);
    if (sexe == "Homme")
        ui->radioButton->setChecked(true);
    else
        ui->radioButton_2->setChecked(true);
    ui->lineEdit_5->setText(age);
    ui->lineEdit_6->setText(NTA);
    ui->dateEdit->setDate(DDP);
    ui->lineEdit_10->setText(montant);
    ui->lineEdit_11->setText(phone);

    // Update button text and connect it to the update function
    QPushButton *editButton = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(row, 10));
    editButton->setText("Update");
    disconnect(editButton, &QPushButton::clicked, nullptr, nullptr); // Disconnect previous signal-slot connection
    connect(editButton, &QPushButton::clicked, [=]() {
        // Retrieve updated data from input fields
        QString updated_id = ui->lineEdit->text();
        QString updated_nom = ui->lineEdit_2->text();
        QString updated_prenom = ui->lineEdit_3->text();
        QString updated_sexe = ui->radioButton->isChecked() ? "Homme" : "Femme";
        QString updated_age = ui->lineEdit_5->text();
        QString updated_NTA = ui->lineEdit_6->text();
        QDate updated_DDP = ui->dateEdit->date();
        QString updated_montant = ui->lineEdit_10->text();
        QString updated_phone = ui->lineEdit_11->text();

        // Update the table widget with the edited data
        ui->tableWidget->item(row, 0)->setText(updated_id);
        ui->tableWidget->item(row, 1)->setText(updated_nom);
        ui->tableWidget->item(row, 2)->setText(updated_prenom);
        ui->tableWidget->item(row, 3)->setText(updated_sexe);
        ui->tableWidget->item(row, 4)->setText(updated_age);
        ui->tableWidget->item(row, 5)->setText(updated_NTA);
        ui->tableWidget->item(row, 6)->setText(updated_montant);
        ui->tableWidget->item(row, 7)->setText(updated_phone);
        ui->tableWidget->item(row, 8)->setText(updated_DDP.toString("yyyy-MM-dd"));

        // Update the database with the edited data
        QSqlQuery query;
        query.prepare("UPDATE INVITE SET id = :id, nom = :nom, prenom = :prenom, sexe = :sexe, age = :age, NTA = :NTA, DDP = :DDP WHERE id = :old_id");
        query.bindValue(":id", updated_id);
        query.bindValue(":nom", updated_nom);
        query.bindValue(":prenom", updated_prenom);
        query.bindValue(":sexe", updated_sexe);
        query.bindValue(":age", updated_age);
        query.bindValue(":NTA", updated_NTA);
        query.bindValue(":age", updated_montant);
        query.bindValue(":phone", updated_phone);
        query.bindValue(":DDP", updated_DDP);

        query.bindValue(":old_id", id); // Use the original ID to identify the row to update


        if (query.exec()) {
            qDebug() << "Row updated in database successfully!";
        } else {
            qDebug() << "Failed to update row in database:" << query.lastError().text();
        }

        // Restore the original button text and connection
        editButton->setText("Edit");
        disconnect(editButton, &QPushButton::clicked, nullptr, nullptr);
        connect(editButton, &QPushButton::clicked, [=]() {
            on_editButton_clicked(row); // Call the slot for editing with the row index
        });
    });

}

void inviteclass::sortTable(const QString &columnName)
{
    // Implement sorting logic here based on the selected column name
    if (columnName == "Aucune") {
        // Do nothing, keep the table as it is
        return;
    } else if (columnName == "sexe") {
        // Sort by sexe column (index 3)
        ui->tableWidget->sortByColumn(3, Qt::AscendingOrder);
    } else if (columnName == "Age") {
        // Sort by Age column (index 4)
        ui->tableWidget->sortByColumn(4, Qt::DescendingOrder);
    } else if (columnName == "Date") {
        // Sort by Date column (index 6)
        ui->tableWidget->sortByColumn(8, Qt::DescendingOrder);
    } else if (columnName == "ID") {
        // Sort by ID column (index 0)
        ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);
    } else if (columnName == "NTA") {
        // Sort by NTA column (index 5)
        ui->tableWidget->sortByColumn(5, Qt::DescendingOrder);
    }else if (columnName == "Montant") {
        // Sort by NTA column (index 7)
        ui->tableWidget->sortByColumn(6, Qt::DescendingOrder);
    }else if (columnName == "Phone") {
        // Sort by NTA column (index 8)
        ui->tableWidget->sortByColumn(7, Qt::DescendingOrder);}
}

void inviteclass::exportTable(const QString &fileName, const QString &fileFormat) {
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    int rowCount = ui->tableWidget->rowCount();
    int columnCount = ui->tableWidget->columnCount();

    if (fileFormat == "PDF") {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);

        QPainter painter;
        painter.begin(&printer);

        // Adjust page size and margins
        QRect rect = painter.viewport();
        QSize size = ui->tableWidget->size();
        size.setWidth(qMax(size.width(), rect.width()));
        size.setHeight(qMax(size.height(), rect.height()));
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(ui->tableWidget->rect());

        // Render the table onto the PDF
        ui->tableWidget->render(&painter);
        painter.end();
    } else if (fileFormat == "CSV") {
        for (int row = 0; row < rowCount; ++row) {
            QStringList rowData;
            for (int col = 0; col < columnCount; ++col) {
                QTableWidgetItem *item = ui->tableWidget->item(row, col);
                if (item) {
                    rowData.append(item->text());
                } else {
                    rowData.append("");
                }
            }
            out << rowData.join(",") << "\n";
        }
    } else {
        qDebug() << "Unsupported file format:" << fileFormat;
    }

    file.close();
}



void inviteclass::exportTableToPDF(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing:" << file.errorString();
        return;
    }

    QPdfWriter pdfWriter(fileName);
    QPainter painter(&pdfWriter);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    int rowCount = ui->tableWidget->rowCount();
    int columnCount = ui->tableWidget->columnCount();
    int cellPadding = 5; // Adjust padding as needed

    QFont font = painter.font();
    font.setPointSize(10); // Adjust font size as needed
    painter.setFont(font);

    qreal x = 10; // X-coordinate to start drawing
    qreal y = 10; // Y-coordinate to start drawing

    qreal cellWidth = 10; // Adjust cell width as needed
    qreal cellHeight = 10; // Adjust cell height as needed

    // Draw table headers
    for (int col = 0; col < columnCount; ++col) {
        QString headerText = ui->tableWidget->horizontalHeaderItem(col)->text();
        painter.drawText(x, y, cellWidth, cellHeight, Qt::AlignCenter, headerText);
        x += cellWidth;
    }

    y += cellHeight; // Move to the next row

    // Draw table content
    for (int row = 0; row < rowCount; ++row) {
        x = 10; // Reset x-coordinate for each row
        for (int col = 0; col < columnCount; ++col) {
            QString cellText = ui->tableWidget->item(row, col)->text();
            painter.drawText(x, y, cellWidth, cellHeight, Qt::AlignCenter, cellText);
            x += cellWidth;
        }
        y += cellHeight; // Move to the next row
    }

    painter.end();
    file.close();

    qDebug() << "PDF exported successfully. Exporting PDF to:" << fileName;
}

// Example of calling exportTableToPDF when the user clicks a button

void inviteclass::on_pushButton_ExportPDF_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save PDF"), "", tr("PDF Files (*.pdf)"));
    if (!filePath.isEmpty()) {
        exportTableToPDF(filePath);
    }
}
void inviteclass::extractAsPdf() {
    QString fileName = "example.pdf"; // Filename only, without the full path
    QString filePath = "C:/Users/Lenovo/Documents/" + fileName; // Full file path
    qDebug() << "Exporting PDF to:" << filePath;
    exportTable(filePath, "PDF"); // Call the exportTable function with the file path and format
    qDebug() << "PDF exported successfully.";
}
void inviteclass::on_comboBox_2_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->label_11->setText("");

    } else if (index == 1) {

        calculateAgeStatistics();
    } else if (index == 2){
           calculateSexStatistics();    }
    else if (index == 3){
              calculateNTAStatistics();    }
    else if (index == 4){
               calculateMntTotalStatistics();    }
}
void inviteclass::calculateAgeStatistics()
{
    // Retrieve age values from the database
    QSqlQuery query("SELECT age FROM INVITE");
    QList<int> ages;
    while (query.next()) {
        ages.append(query.value(0).toInt());
    }

    // Calculate statistics
    QHash<QString, int> ageRanges; // Hash to store age ranges and their counts
    foreach (int age, ages) {
        QString range = getAgeRange(age); // Get age range for the current age
        ageRanges[range]++; // Increment count for the age range
    }

    // Find the most common age range
    QString mostCommonRange;
    int maxCount = 0;
    foreach (QString range, ageRanges.keys()) {
        if (ageRanges[range] > maxCount) {
            mostCommonRange = range;
            maxCount = ageRanges[range];
        }
    }

    // Display the most common age range in QLabel
    ui->label_11->setText("" + mostCommonRange);
}


QString inviteclass::getAgeRange(int age)
{
    if (age >= 17 && age <= 20) {
        return "17-20";
    } else if (age >= 21 && age <= 25) {
        return "21-25";
    } else if (age >= 26 && age <= 30) {
        return "26-30";
    } else {
        return "30+";
    }
}
void inviteclass::calculateSexStatistics()
{
    // Initialize counts
    int femaleCount = 0;
    int maleCount = 0;

    // Iterate over the table rows to count occurrences of "Femme" and "Homme"
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QString sex = ui->tableWidget->item(row, 3)->text(); // Assuming column 3 contains the gender information
        if (sex == "Femme") {
            femaleCount++;
        } else if (sex == "Homme") {
            maleCount++;
        }
    }

    // Debug message to check if counts are working
    if (femaleCount > 0 || maleCount > 0) {
    }

    // Construct the result string
    QString result = QString::number(femaleCount) + " F, " + QString::number(maleCount) + " H";

    // Display the result in QLabel
    ui->label_11->setText(result);
}
void inviteclass::calculateNTAStatistics()
{
    QSqlQuery query("SELECT NTA FROM INVITE");
    int totalNTA = 0;
    while (query.next()) {
        int nta = query.value(0).toInt();
        totalNTA += nta;
    }
    ui->label_11->setText(QString("Total NTA: %1").arg(totalNTA));
}
void inviteclass::calculateMntTotalStatistics()
{
    QSqlQuery query("SELECT montant FROM INVITE");
    double totalMontant = 0.0;
    while (query.next()) {
        double montant = query.value(0).toDouble();
        totalMontant += montant;
    }
    ui->label_11->setText(QString("Total Montant: %1").arg(totalMontant));
}

void inviteclass::addemailButton(int row)
{
    QPushButton *emailButton = new QPushButton("Send Email");
    connect(emailButton, &QPushButton::clicked, [=]() {
    });

    ui->tableWidget->setCellWidget(row, 11, emailButton); // Add the edit button to the table
}
