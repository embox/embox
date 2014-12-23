;; meta.scm -- meta langauge for describing modules
;; Copyright (c) 2009-2012 Alex Shinn.  All rights reserved.
;; BSD-style license: http://synthcode.com/license.txt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; modules

(define *this-module* '())

(define (make-module exports env meta) (vector exports env meta #f))
(define (%module-exports mod) (vector-ref mod 0))
(define (module-env mod) (vector-ref mod 1))
(define (module-env-set! mod env) (vector-set! mod 1 env))
(define (module-meta-data mod) (vector-ref mod 2))
(define (module-meta-data-set! mod x) (vector-set! mod 2 x))

(define (module-exports mod)
  (or (%module-exports mod) (env-exports (module-env mod))))

(define (module-name->strings ls res)
  (if (null? ls)
      res
      (let ((str (cond ((symbol? (car ls)) (symbol->string (car ls)))
                       ((number? (car ls)) (number->string (car ls)))
                       ((string? (car ls)) (car ls))
                       (else (error "invalid module name" (car ls))))))
        (module-name->strings (cdr ls) (cons "/" (cons str res))))))

(define (module-name->file name)
  (string-concatenate
   (reverse (cons ".sld" (cdr (module-name->strings name '()))))))

(define (module-name-prefix name)
  (string-concatenate (reverse (cdr (cdr (module-name->strings name '()))))))

(define load-module-definition
  (let ((meta-env (current-environment)))
    (lambda (name)
      (let* ((file (module-name->file name))
             (path (find-module-file file)))
        (if path (load path meta-env))))))

(define (find-module name)
  (cond
   ((assoc name *modules*) => cdr)
   (else
    (load-module-definition name)
    (cond ((assoc name *modules*) => cdr)
          (else #f)))))

(define (add-module! name module)
  (set! *modules* (cons (cons name module) *modules*)))

(define (delete-module! name)
  (let lp ((ls *modules*) (prev #f))
    (cond ((null? ls))
          ((equal? name (car (car ls)))
           (if prev
               (set-cdr! prev (cdr ls))
               (set! *modules* (cdr ls))))
          (else (lp (cdr ls) ls)))))

(define (symbol-append a b)
  (string->symbol (string-append (symbol->string a) (symbol->string b))))

(define (symbol-drop a b)
  (let ((as (symbol->string a))
        (bs (symbol->string b)))
    (if (and (> (string-length bs) (string-length as))
             (string=? as (substring bs 0 (string-length as))))
        (string->symbol (substring bs (string-length as)))
        b)))

;; (define (warn msg . args)
;;   (display msg (current-error-port))
;;   (display ":" (current-error-port))
;;   (for-each (lambda (a)
;;               (display " " (current-error-port))
;;               (write a (current-error-port)))
;;             args)
;;   (newline (current-error-port)))

(define (to-id id) (if (pair? id) (car id) id))
(define (from-id id) (if (pair? id) (cdr id) id))
(define (id-filter pred ls)
  (cond ((null? ls) '())
        ((pred (to-id (car ls))) (cons (car ls) (id-filter pred (cdr ls))))
        (else (id-filter pred (cdr ls)))))

(define (resolve-import x)
  (cond
   ((not (and (pair? x) (list? x)))
    (error "invalid module syntax" x))
   ((and (memq (car x) '(prefix drop-prefix))
         (symbol? (car (cddr x))) (list? (cadr x)))
    (let ((mod-name+imports (resolve-import (cadr x))))
      (cons (car mod-name+imports)
            (map (lambda (i)
                   (cons ((if (eq? (car x) 'drop-prefix)
                              symbol-drop
                              symbol-append)
                          (car (cddr x))
                          (to-id i))
                         (from-id i)))
                 (or (cdr mod-name+imports)
                     (module-exports (find-module (car mod-name+imports))))))))
   ((and (pair? (cdr x)) (pair? (cadr x)))
    (if (memq (car x) '(only except rename))
        (let* ((mod-name+imports (resolve-import (cadr x)))
               (imp-ids (or (cdr mod-name+imports)
                            (and (not (eq? 'only (car x)))
                                 (module-exports
                                  (find-module (car mod-name+imports)))))))
          ;; (if (not (eq? 'only (car x)))
          ;;     (let ((unbound
          ;;            (id-filter (lambda (i) (not (memq i imp-ids))) (cddr x))))
          ;;       (if (pair? unbound)
          ;;           (warn "import excepting unbound identifiers" unbound))))
          (cons (car mod-name+imports)
                (case (car x)
                  ((only)
                   (cddr x))
                  ((except)
                   (id-filter (lambda (i) (not (memq i (cddr x)))) imp-ids))
                  ((rename)
                   (map (lambda (i)
                          (let ((rename (assq (to-id i) (cddr x))))
                            (if rename (cons (cadr rename) (from-id i)) i)))
                        imp-ids)))))
        (error "invalid import modifier" x)))
   ((find-module x)
    => (lambda (mod) (cons x (%module-exports mod))))
   (else
    (error "couldn't find import" x))))

(define (eval-module name mod . o)
  (let ((env (if (pair? o) (car o) (make-environment)))
        (meta (module-meta-data mod))
        (dir (module-name-prefix name)))
    (define (load-modules files extension fold?)
      (for-each
       (lambda (f)
         (let ((f (string-append dir f extension)))
           (cond
            ((find-module-file f)
             => (lambda (path)
                  (cond (fold?
                         (let ((in (open-input-file path)))
                           (set-port-fold-case! in #t)
                           (load in env)))
                        (else
                         (load path env)))))
            (else (error "couldn't find include" f)))))
       files))
    ;; catch cyclic references
    (module-meta-data-set!
     mod
     `((error "module attempted to reference itself while loading" ,name)))
    (for-each
     (lambda (x)
       (case (and (pair? x) (car x))
         ((import import-immutable)
          (for-each
           (lambda (m)
             (let* ((mod2-name+imports (resolve-import m))
                    (mod2 (load-module (car mod2-name+imports))))
               (%import env (module-env mod2) (cdr mod2-name+imports) #t)))
           (cdr x)))))
     meta)
    (for-each
     (lambda (x)
       (case (and (pair? x) (car x))
         ((include)
          (load-modules (cdr x) "" #f))
         ((include-ci)
          (load-modules (cdr x) "" #t))
         ((include-shared)
          (load-modules (cdr x) *shared-object-extension* #f))
         ((body begin)
          (for-each (lambda (expr) (eval expr env)) (cdr x)))
         ((error)
          (apply error (cdr x)))))
     meta)
    (module-meta-data-set! mod meta)
    (warn-undefs env #f)
    env))

(define (environment . ls)
  (let ((env (make-environment)))
    (for-each
     (lambda (m)
       (let* ((mod2-name+imports (resolve-import m))
              (mod2 (load-module (car mod2-name+imports))))
         (%import env (module-env mod2) (cdr mod2-name+imports) #t)))
     ls)
    env))

(define (load-module name)
  (let ((mod (find-module name)))
    (if (and mod (not (module-env mod)))
        (module-env-set! mod (eval-module name mod)))
    mod))

(define define-library-transformer
  (er-macro-transformer
   (lambda (expr rename compare)
     (let ((name (cadr expr))
           (body (cddr expr))
           (tmp (rename 'tmp))
           (this-module (rename '*this-module*))
           (add-module! (rename 'add-module!)))
       `(let ((,tmp ,this-module))
          (define (rewrite-export x)
            (if (pair? x)
                (if (and (= 3 (length x))
                         (eq? 'rename (identifier->symbol (car x))))
                    (cons (car (cddr x)) (cadr x))
                    (error "invalid module export" x))
                x))
          (define (extract-exports)
            (cond
             ((assq 'export-all ,this-module)
              => (lambda (x)
                   (if (pair? (cdr x))
                       (error "export-all takes no parameters" x))
                   #f))
             (else
              (let lp ((ls ,this-module) (res '()))
                (cond
                 ((null? ls) res)
                 ((and (pair? (car ls)) (eq? 'export (caar ls)))
                  (lp (cdr ls) (append (map rewrite-export (cdar ls)) res)))
                 (else (lp (cdr ls) res)))))))
          (set! ,this-module '())
          ,@body
          (set! ,this-module (reverse ,this-module))
          (,add-module! ',name (make-module (extract-exports) #f ,this-module))
          (set! ,this-module ,tmp))))))

(define-syntax define-library define-library-transformer)
(define-syntax module define-library-transformer)

(define-syntax define-config-primitive
  (er-macro-transformer
   (lambda (expr rename compare)
     `(define-syntax ,(cadr expr)
        (er-macro-transformer
         (lambda (expr rename compare)
           (let ((this-module (rename '*this-module*)))
             `(set! ,this-module (cons ',expr ,this-module)))))))))

(define-syntax orig-begin begin)
(define-config-primitive import)
(define-config-primitive import-immutable)
(define-config-primitive export)
(define-config-primitive export-all)
(define-config-primitive include)
(define-config-primitive include-ci)
(define-config-primitive include-shared)
(define-config-primitive body)
(define-config-primitive begin)

;; The `import' binding used by (chibi) and (scheme base), etc.
(define-syntax repl-import
  (er-macro-transformer
   (let ((meta-env (current-environment)))
     (lambda (expr rename compare)
       (let lp ((ls (cdr expr)) (res '()))
         (cond
          ((null? ls)
           (cons (rename 'orig-begin) (reverse res)))
          (else
           (let ((mod+imps (resolve-import (car ls))))
             (cond
              ((pair? mod+imps)
               (lp (cdr ls)
                   (cons `(,(rename '%import)
                           #f
                           (,(rename 'module-env)
                            (,(rename 'load-module)
                             (,(rename 'quote) ,(car mod+imps))))
                           (,(rename 'quote) ,(cdr mod+imps))
                           #f)
                         res)))
              (else
               (error "couldn't find module" (car ls))))))))))))

(define *modules*
  (list (cons '(chibi) (make-module #f (interaction-environment)
                                    '((include "init-7.scm"))))
        (cons '(scheme) (make-module #f (interaction-environment) '()))
        (cons '(meta) (make-module #f (current-environment) '()))
        (cons '(srfi 0) (make-module (list 'cond-expand)
                                     (current-environment)
                                     (list (list 'export 'cond-expand))))))
