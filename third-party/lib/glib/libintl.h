#ifndef _LIBINTL_H
#define _LIBINTL_H	1

// #include <features.h>

#define __USE_GNU_GETTEXT 1

#define __GNU_GETTEXT_SUPPORTED_REVISION(major) \
  ((major) == 0 ? 1 : -1)

/*
extern char *gettext (const char *__msgid)
     __THROW __attribute_format_arg__ (1);

extern char *dgettext (const char *__domainname, const char *__msgid)
     __THROW __attribute_format_arg__ (2);

extern char *dcgettext (const char *__domainname,
			const char *__msgid, int __category)
     __THROW __attribute_format_arg__ (2);

extern char *ngettext (const char *__msgid1, const char *__msgid2,
		       unsigned long int __n)

extern char *dngettext (const char *__domainname, const char *__msgid1,
			const char *__msgid2, unsigned long int __n)
     __THROW __attribute_format_arg__ (2) __attribute_format_arg__ (3);

extern char *dcngettext (const char *__domainname, const char *__msgid1,
			 const char *__msgid2, unsigned long int __n,
			 int __category)
     __THROW __attribute_format_arg__ (2) __attribute_format_arg__ (3);

extern char *textdomain (const char *__domainname) __THROW;

extern char *bindtextdomain (const char *__domainname,
			     const char *__dirname) __THROW;

extern char *bind_textdomain_codeset (const char *__domainname,
				      const char *__codeset) __THROW;
*/

# define gettext(Msgid) ((const char *) (Msgid))

# define dgettext(Domainname, Msgid) ((void) (Domainname), gettext (Msgid))

# define dcgettext(Domainname, Msgid, Category) \
    ((void) (Category), dgettext (Domainname, Msgid))

# define ngettext(Msgid1, Msgid2, N) \
    ((N) == 1 \
     ? ((void) (Msgid2), (const char *) (Msgid1)) \
     : ((void) (Msgid1), (const char *) (Msgid2)))

# define dngettext(Domainname, Msgid1, Msgid2, N) \
    ((void) (Domainname), ngettext (Msgid1, Msgid2, N))

# define dcngettext(Domainname, Msgid1, Msgid2, N, Category) \
    ((void) (Category), dngettext (Domainname, Msgid1, Msgid2, N))

# define textdomain(Domainname) ((const char *) (Domainname))

# define bindtextdomain(Domainname, Dirname) \
    ((void) (Domainname), (const char *) (Dirname))

# define bind_textdomain_codeset(Domainname, Codeset) \
    ((void) (Domainname), (const char *) (Codeset))

#endif
