/*!
 * @file utils.h
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Set of static utility functions used in other classes
 */

#ifndef E_UTILS_H
#define E_UTILS_H

#include <QPixmap>
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
     * @brief Read file into ByteArray
     * @param fileName File name
     * @returns Byte Array (empty when failed to read the file)
     */
    static QByteArray readFile(QString fileName); 

    /*!
     * @brief Write byte array to file
     * @param fileName File name
     * @param data Byte array
     * @returns true on success, false otherwise
     */
    static bool writeFile(QString fileName, QByteArray data);

    /*!
     * @brief Open FileDialog and then read that file into ByteArray
     * @param parent Parent widget of the created FileDialog
     * @param caption FileDialog caption
     * @param filter FileDialog filter
     * @returns Byte Array (empty when failed to read the file)
     */
    static QByteArray loadFile(QWidget *parent, QString caption, QString filter);

    /*!
     * @brief Open FileDialog and then save byte array into that file
     * @param parent Parent widget of the created FileDialog
     * @param data Byte array
     * @param caption FileDialog caption
     * @param filter FileDialog filter
     * @returns true on success, false otherwise
     */
    static bool saveFile(QWidget *parent, QByteArray data, QString caption, QString filter);

    /*!
     * @brief Open image in new window
     * @param image Pixmap
     * @param parent Parent widget of the created window
     * @returns
     */
    static void openImage(QPixmap image, QWidget *parent);

    /*!
     * @brief Open text in new window
     * @param text Text
     * @param parent Parent widget of the created window
     * @returns
     */
    static void openText(QString text, QWidget *parent);

    /*!
     * @brief Center widget relative to another widget
     * @param widget Widget to be centered
     * @param parent Relative to which widget
     * @returns
     */
    static void centerWidget(QWidget *widget, QWidget *parent);
};

#endif