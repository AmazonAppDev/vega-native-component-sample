// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

// Sample content: a subset of https://giolaq.github.io/scrap-tv-feed/catalog.json
// embedded statically so the sample does not depend on a network fetch for its
// text. Only the images are downloaded at runtime (by the native ImageCard).
export interface CatalogItem {
  id: string;
  title: string;
  meta: string;
  url: string;
  description: string;
}

export const catalog: CatalogItem[] = [
  {
    id: 'vega',
    title: 'Aliens From Vega',
    meta: 'Sci-Fi • 2022',
    url: 'https://giolaq.github.io/scrap-tv-feed/content/aliens-from-vega/vega.jpg',
    description:
      'Gio and Eric befriend aliens coming from Vega and save the earth',
  },
  {
    id: 'the-accounting-cats',
    title: 'The Accounting Cats',
    meta: 'Comedy / Workplace • 2020',
    url: 'https://giolaq.github.io/scrap-tv-feed/content/the-accounting-cats/poster_1920x1080.jpg',
    description:
      'Ragdoll Calculator and her team of accounting cats handle corporate finances with meticulous attention to detail. Features cats wearing tiny glasses and visors, working with calculators and spreadsheets, though they occasionally get distracted by moving cursor arrows on computer screens.',
  },
  {
    id: 'parking-wars',
    title: 'Parking Wars',
    meta: 'Comedy / Reality • 2020',
    url: 'https://giolaq.github.io/scrap-tv-feed/content/parking-wars/poster_1920x1080.jpg',
    description:
      'Follow the daily adventures of gnome parking enforcement officers as they patrol city streets, issue tiny tickets, and maintain order in parking lots. Chief Gnome Officer Tiny Ticket leads his team of dedicated gnomes through the challenges of urban parking enforcement.',
  },
  {
    id: 'the-beige-knight',
    title: 'The Beige Knight',
    meta: 'Action / Comedy • 2021',
    url: 'https://giolaq.github.io/scrap-tv-feed/content/the-beige-knight/poster_1920x1080.jpg',
    description:
      "By day, Bruce Neutral is a mild-mannered interior designer. By night, he becomes The Beige Knight, using his mastery of neutral tones to calm criminals and restore peace to Gotham's most colorful neighborhoods.",
  },
  {
    id: 'the-endless-queue',
    title: 'The Endless Queue',
    meta: 'Thriller / Horror • 2019',
    url: 'https://giolaq.github.io/scrap-tv-feed/content/the-endless-queue/poster_1920x1080.jpg',
    description:
      'When people join a line for an unknown purpose and discover they can never leave, psychological horror ensues. A mind-bending thriller about conformity, patience, and the dark side of waiting.',
  },
  {
    id: 'cereal-streamz',
    title: 'Cereal Streamz',
    meta: 'Kids / Reality • 2024',
    url: 'https://giolaq.github.io/scrap-tv-feed/content/cereal-streamz/poster_1920x1080.jpg',
    description:
      'Crunch Time livestreams daily cereal tastings with chat interaction, cereal mixing experiments, and guest appearances from other cereal influencers. Kids tune in to watch their favorite streamers try new cereals, rate combinations, and react to viewer suggestions in real-time.',
  },
];
