/*!
 * @file utils.h
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Set of static utility functions used in other classes
 */

#ifndef E_UTILS_H
#define E_UTILS_H

#include <QWidget>
#include <QString>
#include <QByteArray>

/*!
 * @brief Set of utility functions that other classes can work with
 */
class Utils
{
public:
    /*!
     * @brief Reads content of provided file into ByteArray
     * @param fileName Path to file
     * @returns QByteArray representing file contents (empty when failed to read file)
     */
    static QByteArray readFile(QString fileName); 

    /*!
     * @brief Prompts user to select file and then reads content of that file
     * @param parent Parent widget of the created FileDialog
     * @returns QByteArray representing file contents (empty when failed to read file or no file selected)
     */
    static QByteArray loadFile(QWidget *parent, QString name, QString filter);

    /*!
     * @brief Center widget relative to another widget
     * @param widget Widget to be centered
     * @param parent To center of which widget should the widget be centered
     */
    static void centerWidget(QWidget *widget, QWidget *parent);
};

#endif