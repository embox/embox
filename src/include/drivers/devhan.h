/**
 * @file
 *
 * @brief device & handler implementation
 *
 * @date Jul 05 2010
 *
 * @auther Michail Skorginskii
 */

/* device */
struct device {
	int id_handler;
	/* args wtf!? */
	void (* handler) ( void );
};


