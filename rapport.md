# Gestion d'accès concurrents

Auteurs: Koestli Camille, Nicolet Victor

## Description des fonctionnalités du logiciel

Ce projet consiste à implémenter un système hospitalier devant gérer le flux de patients et les besoins en ressources. Il inclut différents acteurs : les ambulances, les cliniques spécialisées, les hôpitaux et les fournisseurs de matériel médical.

Chaque acteur est exécuté en tant que thread et doit interagir avec les autres pour assurer une gestion efficace des ressources et des patients.

## Choix d'implémentation

Le premier élément important à comprendre est que les ressources et les patients ne peuvent être uniquement modifiés que par leur propriétaire, car ce sont des attributs privés. L'accès concurrent à ces variables est causé par les multiples entités dans différents threads qui demandent une transaction en parallèle. La transaction (lecture et modification des fonds et des ressources) doit être effectuée de manière atomique, c'est pourquoi nous avons utilisé un mutex par entité.

### Mutex

Nous avons implémenté un mutex d'accès protégé dans la classe `Seller`, qui fournit à chaque sous-classe son propre mutex exclusif.Pour simplifier notre implémentation et améliorer les performances, nous avons opté pour un seul mutex pour protéger les deux variables. Ce mutex est acquis lorsque nous lisons ou modifions les fonds ou les ressources, puis est libéré une fois les opérations terminées.

- Transactions entre acteurs : Chaque fois qu’un transfert de patient ou de ressource a lieu, la section critique met à jour les stocks et les fonds.
-Accès aux ressources de chaque thread : Lorsque les stocks ou fonds sont modifiés, un mutex assure que seule une ambulance ou une clinique à la fois peut réaliser cette opération.

### Clinic

La Clinic est responsable de traiter les patients malades en fonction des ressources et de les commander en cas de besoin.

`Clinic::request`

- Cette méthode permet à d’autres entités de demander des patients sains. La clinique vérifie que le type de patient demandé est bien `PatientHealed` et retourne la quantité demandée pour effectuer le transfert.
- Si les conditions sont remplies, la clinique met à jour son stock et ses fonds, et retourne 1 pour indiquer que la demande est ok. Sinon, la méthode retourne 0.
- La section critique est protégée par un mutex pour garantir une modification sûre des ressources et des fonds en cas d’accès concurrent.

`Clinic::treatPatient()`

- Cette méthode traite un patient malade pour le transformer en patient guéri, mais seulement si les ressources nécessaires sont disponibles (vérifiées par `verifyResources()`).
- En cas de succès du traitement, le stock de `PatientHealed` est augmenté et celui de `PatientSick` est décrémenté. Le traitement est simulé par un appel à `interface->simulateWork()` qui représente le temps requis pour soigner un patient.
- Dans cette méthode, un mutex protège la modification des stocks et des fonds, pour éviter les conflits de concurrence.

`Clinic::orderResources()`

- La clinique commande des ressources aux fournisseurs en cas de besoin. Si des ressources nécessaires sont épuisées et que les fonds sont suffisants, les ressources manquantes, comme Pill, Scalpel, et Syringe, sont commandées chez un fournisseurs aléatoires, et les patients malades peuvent être demandés aux hôpitaux.
- La section critique est protégée par un mutex pour garantir que les modifications des stocks et des fonds se font sans interférence.

`Clinic::run()`

- La méthode `run()` est la boucle principale de la clinique, qui vérifie constamment si elle dispose de suffisamment de ressources pour traiter un patient. Si les ressources sont disponibles, elle appelle `treatPatient()`. Sinon, elle appelle `orderResources()` pour approvisionner les stocks.
- La boucle continue tant que `stopRequest` est `false`, ce qui permet d’arrêter proprement la simulation lorsque le signal est émis.

### Ambulance

L’Ambulance est chargée de transporter les patients malades vers les hôpitaux.

`Ambulance::sendPatient()`

- Cette méthode sélectionne un hôpital au hasard parmi ceux disponibles et tente de transférer un patient malade. Avant, elle vérifie si l'hôpital a la capacité d’accueillir un nouveau patient et si le montant de la transaction peut être payé.
- Si le transfert est possible, le nombre de patients dans l’ambulance est décrémenté et les fonds sont mis à jour.
- La méthode utilise un mutex pour sécuriser les modifications des attributs `money`, `stocks`, et `nbTransfer`.

`Ambulance::run()`

- Cette méthode est la boucle principale de l'ambulance, qui continue à exécuter la routine de transfert de patients tant que la variable `stopRequest` n’est pas activée.
- La méthode vérifie d’abord s’il y a des patients malades dans le stock de l’ambulance. Si oui, elle appelle `sendPatient()` pour tenter un transfert.

### Hospital

`Hospital::freeHealedPatient()`

`Hospital::transferPatientsFromClinic()`

`Hospital::send`

`Hospital::run()`

### Supplier

`Supplier::request`

`Supplier::run()`

### Fin simulation
Afin d'assurer un arrêt correct de la simulation, nous avons déclaré une variable booléenne appelée `stopRequest` dans le fichier `Utils.cpp`, qui sert de signal pour indiquer si une demande d'arrêt de la simulation a été faite. Cette variable est initialement définie sur `false` et passe à `true` si un appel à `Utils::endService()` est effectué (lorsque la fenêtre est fermée).

Pour en tenir compte dans le reste du projet, nous avons ajouté une boucle `while` qui vérifie si `stopRequest` est `true` dans toutes les fonctions `run` de chaque sous-classe de `Seller`.

## Tests effectués

Pour identifier les zones critiques, nous avons testé notre programmes dans différentes situations :

- Tests de concurrence :
- Tests unitaires :

## Conclusion

Notre objectif principal était de gérer efficacement la concurrence dans notre application de simulation d'un hôpital et de ventes dynamiques. Pour y parvenir, nous avons créé un système fiable afin de protéger les ressources cruciales dans un environnement multi-thread en plaçant stratégiquement plusieurs mutex autour des sections critiques du code.

Ce système garantit que chaque entité a un accès exclusif à ses ressources essentielles, évitant ainsi les conflits et assurant la cohérence des données. Notre approche met également l’accent sur l’efficacité en minimisant les retards dans l’exécution du programme, en particulier lors de la gestion des fonctions de transaction.
